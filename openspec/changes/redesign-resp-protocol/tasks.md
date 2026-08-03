## 1. Value model and pure protocol layer

- [x] 1.1 Replace `common/include/common/protocol/resp_data_type.h` with `resp_value.h` defining `RESPError` (code+message), `RESPValue` (`Type` enum + payload variant) and the RESP2 wire-prefix enum; remove `resp_data_type.h` and `KiedisSupportType`
- [x] 1.2 Add pure `encode(const RESPValue&) -> std::string` covering every RESP2 type (simple string, error, integer, bulk string, null → `$-1\r\n`, array incl. empty and nested) in `protocol.h`/`protocol.cpp`
- [x] 1.3 Add pure line-parsing helpers (`parse_simple_string`, `parse_error`, `parse_integer`, `parse_length`) returning `SyncResult`, using bounded manual integer parsing (sign/digits/overflow → `protocol_error`) with no throwing `stoi`/`stoll`
- [x] 1.4 Add a wire-prefix dispatch helper mapping a first byte to a RESP2 type

## 2. Reader/writer I/O adapters

- [x] 2.1 Rewrite `RESPReader` as a thin awaitable orchestrator: `read() -> AsyncResult<RESPValue>` via `read_line`/`read_exact`, recursive arrays, `$-1`/`*-1` → `Null`, EOF → `eof`/`connection_reset` result, malformed input → `protocol_error`; remove exception throws from protocol paths
- [x] 2.2 Rewrite `RESPWriter`: `write(const RESPValue&) -> AsyncResult<std::size_t>` = `encode()` + `Connection::write`; retain convenience `write_*` delegates returning results
- [x] 2.3 Update `reader.h`/`writer.h`; remove dead API (`read_request` tuple form, `read_array_item`, unused members)

## 3. Command encoding

- [x] 3.1 Rework `to_request_string` in `common/include/common/command/command.h` to build a `RESPValue` array of bulk strings and return `encode(...)` so requests are correctly framed

## 4. Server connection handler

- [x] 4.1 Rewrite `handle_connection`: loop reading `RESPValue`; EOF ends loop without reply; `protocol_error` → `-ERR Protocol error...` + close; array/bulk-string requests dispatched; other types → `-ERR` unknown request type
- [x] 4.2 Wire `handle_request` to the `command_handler` map (uppercase command name, lookup, `-ERR` when unregistered); update includes to `resp_value.h`
- [x] 4.3 Confirm `server-connection-handler` spec scenarios hold (unsupported request type, unregistered command, disconnect ends session, malformed → error reply + close)

## 5. Client command execution

- [x] 5.1 Update `KiedisClient::get` to build request via `Command<Get>::to_string` → `encode()` → write → read → return `AsyncResult<RESPValue>`; update `kiedis.h` signature and drop `KiedisSupportType`

## 6. Tests

- [ ] 6.1 Add `common/test/protocol/reader_test.cpp`: pure helpers across the type universe incl. `$-1`, `*-1`, `-ERR msg` code/message split, `:-123`, garbage and overflow → `protocol_error`
- [ ] 6.2 Add `common/test/protocol/writer_test.cpp`: `encode()` byte-exactness for every type incl. empty/binary payloads and empty arrays
- [ ] 6.3 Add `common/test/protocol/roundtrip_test.cpp`: `encode` → pure-parse → value equality across the universe
- [ ] 6.4 Add a loopback socket fixture (one `io_context`, localhost listener + connect pair) and `loopback_test.cpp` exercising `RESPWriter.write()`/`RESPReader.read()` and `Connection` `read_until`/`read_exact`/`write`
- [ ] 6.5 Rewrite `connection_test.cpp` as real gtest using the loopback fixture, removing external-host HTTP calls; register protocol tests in `common/test/CMakeLists.txt`
- [ ] 6.6 Verify `ctest` runs all tests clean under ASan/`--coverage` with no warnings or leaks

## 7. Build and validation

- [ ] 7.1 Full Debug build via `script/build_server.sh`/`script/build_client.sh` (or conan presets); fix `-Werror`/sanitizer issues
- [ ] 7.2 Run the full test suite (`ctest --preset conan-debug`) and confirm every spec scenario is exercised
- [ ] 7.3 Run `openspec validate --change redesign-resp-protocol`

Note: a reply-size-limit guard (`proto-max-bulk-len`-style) is intentionally deferred — it is the design's Open Question and does not change this task breakdown or the API shape.
