## Why

Current RESP handling in common/protocol is incomplete and has correctness gaps (null bulk handling and limited array support). These gaps can cause interoperability and robustness issues between client and server and make fuzzing or malformed inputs unsafe.

## What Changes

- Harden RESP2 reader and writer to fully-support RESP2 semantics (nulls, nested/mixed arrays, correct integers, strict CRLF handling).
- Add a typed representation for nil and nested arrays.
- Add comprehensive unit and integration tests (round-trip, fuzz cases).
- Maintain backward-compatible convenience APIs.

## Capabilities

### New Capabilities
- `resp2-protocol`: Precise RESP2 protocol support: correct parsing/serialization of RESP2 frames including $-1/*-1 nulls, nested/mixed arrays, integers, and robust error handling.

### Modified Capabilities
- None.

## Impact

Affected components:
- common/protocol (reader, writer, types)
- client and server components that use the common RESP APIs (round-trip tests)
- CMake test targets and test cases

This is backwards-compatible at public convenience API level; internal representation will expand to include explicit nils and nested arrays.
