## Why

The RESP layer in `common/` cannot represent null replies or nested arrays, flattens array items to `string`, and contains bugs that desynchronize the byte stream (`$-1` and `*-1` are mis-parsed, `read_error` mis-slices). `command.h` emits malformed requests (`*2\r\nGET\r\nkey\r\n` with no bulk-string framing), so the client cannot talk to the server or to real Redis. The layer must be redesigned before any commands can be built on top.

## What Changes

- Introduce a single nested, type-preserving `RESPValue` value model (RESP2) used by both reader and writer, replacing the `{variant, RESPDataType}` tuple and the unused `KiedisSupportType`. **BREAKING**
- Split pure protocol logic from I/O: `encode()` produces wire bytes; pure line-parsing helpers consume a line string; thin awaitable `RESPReader`/`RESPWriter` adapters orchestrate over `Connection`. **BREAKING**
- Correctly handle the full RESP2 type universe: simple string, error (code/message), integer, bulk string, null bulk string (`$-1`), array (nested, type-preserving), null array (`*-1`); `$-1` and `*-1` map to a single `Null`.
- Report errors via result types (`AsyncResult<T>`), not exceptions: EOF → `eof`/`connection_reset`, malformed or out-of-range input → `protocol_error`. No uncaught `stoi`/`stoll` exceptions.
- Route `command.h` request building through `encode()` so command requests are always correctly framed bulk strings.
- Rewrite the server connection handler: loop until EOF, dispatch arrays to the command map, reply `-ERR` on unsupported/unknown request types, reply `-ERR Protocol error...` and close on malformed input. Wire the existing (currently empty) `command_handler` map. **BREAKING**
- Wire `KiedisClient::get()` to perform a real round-trip: build request → `encode()` → write → read reply → return `RESPValue`.
- Add a gtest protocol suite (pure encode/parse unit tests, round-trip, loopback) and replace the network-dependent `connection_test.cpp` with a loopback-based Connection test.

## Capabilities

### New Capabilities
- `resp-protocol`: RESP2 value model (`RESPValue`), pure `encode()`/parse helpers, reader/writer I/O adapters with result-based error semantics, null handling, and correct command request encoding.
- `server-connection-handler`: server request loop behavior — reading `RESPValue` requests, EOF/error handling, request-type dispatch to the command map, protocol-error reply-and-close.
- `client-command-execution`: client command execution — `get()` sends a correctly framed request and parses the reply.

### Modified Capabilities
<!-- No existing specs in openspec/specs/; all capabilities are new. -->

## Impact

- `common/include/common/protocol/resp_data_type.h` — replaced by `RESPValue` (resp_data_type or new header).
- `common/src/protocol/reader.cpp`, `common/src/protocol/writer.cpp` — rewritten as thin I/O adapters over pure `encode`/parse helpers.
- `common/include/common/command/command.h` — request framing routed through `encode()`.
- `server/src/kiedis/connection_handler.cpp` — rewritten request loop; `server/include/server/commands/commands.h` `command_handler` map wired (contents stay empty; real commands are a follow-up change).
- `client/src/kiedis.cpp` — `get()` performs a real round-trip; return type becomes `AsyncResult<RESPValue>`.
- `common/test/` — new `protocol/` gtest suite; `connection/connection_test.cpp` replaced with loopback test.
- No new dependencies; still C++20, Boost.ASIO, gtest.
