## Context

The RESP layer (`common/src/protocol/reader.cpp`, `writer.cpp`) parses wire bytes inside awaitable coroutines glued to `Connection`, flattens arrays to `string`, cannot represent null, and mis-parses null bulk strings/arrays, causing stream desync. `command.h` builds requests without bulk-string framing. The only consumers are the server handler (`connection_handler.cpp`, a stub) and the client skeleton (`kiedis.cpp:get()`, returns 0). The codebase convention (AGENTS.md, `types.h`) is result-based error handling (`boost::outcome`), C++20, coroutines, `-Werror` + ASan + `--coverage` under `ENABLE_TEST`. See proposal.md - Why.

## Goals / Non-Goals

**Goals:**
- A single, testable, type-preserving RESP2 value model shared by reader and writer.
- Pure, socket-free protocol logic (encode/parse) so the type universe is unit-testable.
- Result-based error semantics (no exceptions) with EOF and protocol errors distinguished.
- Protocol-correct server request loop and a working client round-trip.

**Non-Goals:**
- RESP3 support (RESPValue is extensible; shape change only).
- Implementing real commands (GET/SET/...); the command map stays empty.
- Concurrency/pipelining, connection pooling, or reply size limits (e.g. `proto-max-bulk-len`).
- TLS, persistence, transactions.

## Decisions

### D1: Single tagged `RESPValue` value model
Both directions share one type. Shape:

```cpp
struct RESPError { std::string code; std::string message; };
struct RESPValue {
    enum class Type { SimpleString, Error, Integer, BulkString, Null, Array };
    Type type;
    std::variant<std::monostate, std::string, std::int64_t, RESPError, std::vector<RESPValue>> data;
};
```

`$-1` and `*-1` both decode to `Null`. Simple strings and bulk strings stay distinct alternatives (both hold `std::string`) because their wire format and reply semantics differ.

*Alternatives considered:* keep `{variant, RESPDataType}` tuple (rejected — type lives outside the value, easy to misalign); variant alternatives with wrapper structs like `struct SimpleString { std::string v; }` (rejected — verbose to construct/switch; an explicit `Type` enum + payload variant is easier to match on server-side).

### D2: Pure encode/parse split with `SyncResult`
Protocol logic is pure and socket-free:

```
WRITER   encode(const RESPValue&)  -> std::string
READER   parse_simple_string(line) -> SyncResult<RESPValue>
         parse_error(line)        -> SyncResult<RESPValue>
         parse_integer(line)      -> SyncResult<RESPValue>
         parse_length(line)       -> SyncResult<std::int64_t>   // $n / *n headers, -1 = null
```

The pure layer (encode + parse helpers + wire-prefix dispatch) lives in `protocol.h`/`protocol.cpp`; `resp_value.h`/`.cpp` holds only the value model.

All pure helpers return `SyncResult<T>` (`boost::outcome_v2::result<T>`, aliased in `types.h`) so the pure layer and the awaitable layer share one error vocabulary: `errc::protocol_error` for malformed/overflow, `errc::eof` for end of stream. `std::stoll` is replaced by bounded manual parsing (optional sign, digits, overflow check) — never throws.

The awaitable `RESPReader::read()` is a thin orchestrator: `read_line()` (delimiter-inclusive, as `Connection::read_until` already returns) → dispatch on first char → pure helper → for non-null `$n`/`*n`, `read_exact` payload plus the trailing `\r\n`. Arrays recurse on `read()` to build nested `RESPValue`s. Line helpers strip the trailing `\r\n`.

`RESPWriter::write(const RESPValue&)` = `encode()` + `Connection::write`, returning `AsyncResult<std::size_t>`. Convenience methods (`write_simple_string`, `write_bulk_string`, `write_integer`, `write_error`, `write_null_bulk_string`, `write_array`) are retained as thin delegates so the existing `command_handler` signature (`RESPWriter&`) and future commands stay convenient.

*Alternatives considered:* keep parsing inside coroutines and test via sockets only (rejected — each null/error case needs a socket dance, bugs harder to isolate); a streaming buffer-consumer parser like real decoders (rejected for now — more churn on `Connection`'s buffer; line-helper split covers all bug-prone cases).

### D3: Error model — results, not exceptions
`read()` returns `AsyncResult<RESPValue>`. EOF/connection reset propagate as `eof`/`connection_reset` from `Connection`, letting the server end the loop without a reply. Malformed input produces `protocol_error`. The writer propagates write errors. No `throw` in protocol code paths; the existing `catch` in the handler remains only as a defensive last resort.

### D4: Header consolidation
Replace `resp_data_type.h` (enum + unused `KiedisSupportType`) with `resp_value.h` defining `RESPError`, `RESPValue`, and the wire-prefix enum (renamed `RESP2Type`), plus `protocol.h` for the pure encode/parse/dispatch functions. `resp_data_type.h`, `KiedisSupportType`, and the throw-based reader are removed. **BREAKING** — consumers (`connection_handler.cpp`, `kiedis.cpp`) update in this change.

### D5: Command encoding via `encode()`
`to_request_string` in `command.h` builds a `RESPValue` array (command text + args, all bulk strings) and calls `encode()`. Numeric option args keep the existing `std::to_string` fold. This makes `command.h` depend on `common/protocol` — acceptable, it's the same library. Single source of truth for wire bytes.

### D6: Server loop
`handle_connection` becomes:

```
loop:
  read() -> RESPValue
    error:
      eof/connection_reset -> break                    // normal disconnect
      protocol_error       -> write_error("ERR", "Protocol error..."); close(); break
    value:
      Array      -> flatten bulk-string items to args -> handle_request(args)
      BulkString -> handle_request({payload})
      _          -> write_error("ERR", "unknown request type")
```

`handle_request` uppercases `args[0]`, looks it up in `command_handler`, and replies `-ERR` when absent (the map stays empty this change). Array items that are not bulk strings are treated as an unknown-request error.

### D7: Client round-trip
`KiedisClient::get(key)` builds the request via `Command<Get>::to_string(key)` → `encode()` → `conn.write` → `RESPReader::read()` → returns `AsyncResult<RESPValue>`. `KiedisSupportType` return is replaced by `RESPValue`.

### D8: Testing strategy
gtest suite under `common/test/protocol/` (gated by `ENABLE_TEST`; must stay warning-free under `-Werror`/ASan/coverage):
- `reader_test.cpp` — pure helpers over the type universe incl. `$-1`, `*-1`, `-ERR x` code/message split, `:-123`, garbage/overflow → `protocol_error`.
- `writer_test.cpp` — `encode()` byte-exactness for every type incl. empty/binary payloads and empty arrays.
- `roundtrip_test.cpp` — `encode` → pure-parse → value equality across the universe.
- `loopback_test.cpp` — real socket pair on localhost: `RESPWriter.write()` → `RESPReader.read()` → equal (exercises the awaitable adapters).

`connection_test.cpp` (currently an external-host smoke test dialing github.com/baidu) is rewritten as a real gtest using a localhost socket pair to exercise `read_until`/`read_exact`/`write` round-trips — removing the CI network dependency. A shared loopback fixture (listener + connect pair in one `io_context`) serves both tests.

## Risks / Trade-offs

- [Stream desync regressions] → null handling (`$-1`/`*-1`) is explicitly unit-tested; round-trip and loopback tests cover the whole universe.
- [Unbounded bulk lengths enable memory exhaustion] → mitigated by design scope: declared non-goal; a `proto-max-bulk-len`-style guard can be added later without spec/API change (Open Question below).
- [`command.h` now depends on `protocol`] → both are in `reddish_hermit_common`; no new layering violation, documented in D5.
- [Folded `*-1` into `Null` loses the distinction between null-bulk and null-array on the wire] → RESP2 treats both as null; writer emits `$-1\r\n` for `Null`, matching Redis reply conventions.
- [`-Werror` + ASan on tests] → tests must avoid warnings/leaks; loopback fixture tears down sockets deterministically.

## Migration Plan

Single change; no data or schema to migrate. Build via `conan install` → `cmake --preset conan-debug` → `cmake --build` → `ctest`. Rollback is `git revert` of the change's commits (all in-repo source).

## Open Questions

- **Reply size limit:** should the reader enforce a maximum bulk-string/array length (mirroring Redis `proto-max-bulk-len`)? Deferrable — a guard can be added behind the same `read()` signature later; it would only change behavior for oversized inputs, not the spec/API shape.
