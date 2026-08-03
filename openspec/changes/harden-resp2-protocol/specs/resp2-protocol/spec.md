## Purpose

Provide a precise, testable RESP2 protocol behavior contract for the project: parsing and serializing RESP2 frames (simple string, error, integer, bulk string, arrays) including nulls, nested/mixed arrays, and robust error handling.

## ADDED Requirements

### Requirement: Integer parsing
The system SHALL parse RESP2 integer frames (prefix ':') and return the exact integer value including negatives (e.g., ":-42\r\n" => -42).

#### Scenario: Parse negative integer
- **WHEN** the reader receives ":-42\r\n"
- **THEN** it returns an integer value -42 and RESPDataType::Integer

### Requirement: Bulk string null handling
The system SHALL interpret a bulk string frame with length -1 as a null value ("$-1\r\n").

#### Scenario: Null bulk string
- **WHEN** the reader receives "$-1\r\n"
- **THEN** it returns an explicit null representation (not empty string) and RESPDataType::BulkString/null

### Requirement: Nested and mixed-type arrays
The system SHALL parse arrays that contain mixed types (simple strings, integers, bulk strings, nulls) and nested arrays to arbitrary depth up to configured limits.

#### Scenario: Nested mixed array
- **WHEN** the reader receives "*3\r\n$3\r\nfoo\r\n:7\r\n*2\r\n$-1\r\n+OK\r\n"
- **THEN** it returns a top-level array with ["foo", 7, [null, "OK"]]

### Requirement: Writer correctness and round-trip
The writer SHALL serialize values such that a reader round-trip recovers the original typed value (including nulls and nested arrays) subject to configured limits.

#### Scenario: Round-trip
- **WHEN** a complex value (nested array with ints, nulls, strings) is written by the writer and then read by the reader
- **THEN** the reader returns an equivalent typed value

### Requirement: Malformed frames detection
The system SHALL detect and reject malformed frames (bad CRLF placement, length mismatch in bulk strings, invalid type prefix) by raising a well-defined parsing error.

#### Scenario: Invalid bulk length
- **WHEN** reader sees "$5\r\nabc\r\n"
- **THEN** reader raises a parse error indicating length mismatch

### Requirement: Resource limits
The system SHALL enforce configurable limits: maximum array recursion depth and maximum bulk string size. Exceeding limits SHALL produce a parse error rather than unbounded resource consumption.

#### Scenario: Excessive depth
- **WHEN** an input exceeds max configured array depth
- **THEN** parser errors with 'max depth exceeded'


## Notes
- Use SHALL/MUST verbs for normative requirements.
- Each requirement is testable and maps to unit/integration tests in tasks.md.
