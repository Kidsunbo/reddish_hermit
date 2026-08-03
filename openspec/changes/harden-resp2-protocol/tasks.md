## 1. Setup & types

- [ ] 1.1 Add RESPValue variant type and config constants (MAX_BULK_SIZE, MAX_ARRAY_DEPTH) in common/include/common/protocol
- [ ] 1.2 Add ParseError type for protocol parsing errors

## 2. Reader improvements

- [ ] 2.1 Add validation tests for integer parsing (positive, negative, overflow, malformed)
- [ ] 2.2 Update read_bulk_string to detect $-1 null and validate length matches
- [ ] 2.3 Implement recursive read_array supporting mixed types and nested arrays with depth checks
- [ ] 2.4 Add strict CRLF validation in read_line/read_exact

## 3. Writer improvements

- [ ] 3.1 Implement writer for RESPValue variant, support nulls and nested arrays
- [ ] 3.2 Add convenience writer overloads for backward compatibility

## 4. Tests

- [ ] 4.1 Unit tests: integer/bulk/null/malformed cases in common/test/protocol
- [ ] 4.2 Round-trip tests: writer->reader equivalence for nested mixed values
- [ ] 4.3 Integration test: client/server handshake round-trip using RESP frames
- [ ] 4.4 Fuzz harness or property tests for robustness (optional)

## 5. Integration & cleanup

- [ ] 5.1 Update calling code where read_request return type changed (wrap existing callers with conversion helpers)
- [ ] 5.2 Run full test suite and fix regressions
- [ ] 5.3 Document the behavior in AGENTS.md or README where protocol is mentioned


Notes:
- Tasks are ordered by dependency. Each should be small and verifiable. Resolve any design open questions before starting implementation.
