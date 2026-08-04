## Purpose

Defines the RESP2 wire protocol layer: a single type-preserving value model, correct encode/decode of every RESP2 type including null, and result-based error semantics for the reader and writer.

### Requirement: RESP2 value model
The system SHALL represent RESP2 values with a single nested, type-preserving value type capable of distinguishing simple strings, errors, integers, bulk strings, null, and arrays, where arrays contain values of the same type. Null bulk strings (`$-1`) and null arrays (`*-1`) SHALL both decode to a single null value.

#### Scenario: Decode null bulk string
- **WHEN** the reader encounters the wire bytes `$-1\r\n`
- **THEN** it produces a null value and does not consume any bytes of the following message

#### Scenario: Decode null array
- **WHEN** the reader encounters the wire bytes `*-1\r\n`
- **THEN** it produces a null value that is distinguishable from an empty array

#### Scenario: Nested array preserves element types
- **WHEN** the reader decodes an array whose items include an integer and a nested array
- **THEN** each item is represented by its own typed value and the nesting is preserved

#### Scenario: Integer and error types carry structure
- **WHEN** the reader decodes `:-123\r\n` or `-ERR msg\r\n`
- **THEN** the integer is the value -123 and the error carries its code (`ERR`) and message (`msg`) separately

### Requirement: Correct RESP2 encoding
The system SHALL encode RESP2 values into exact wire bytes: simple strings as `+...\r\n`, errors as `-<code> <message>\r\n`, integers as `:<n>\r\n`, bulk strings as `$<len>\r\n<bytes>\r\n`, null as `$-1\r\n`, and arrays as `*<count>\r\n` followed by each encoded element. The encoded size SHALL match the byte length, not the character length.

#### Scenario: Encode a simple string
- **WHEN** a simple string value `OK` is encoded
- **THEN** the result is exactly `+OK\r\n`

#### Scenario: Encode a bulk string
- **WHEN** a bulk string value `hello` is encoded
- **THEN** the result is exactly `$5\r\nhello\r\n`

#### Scenario: Encode an empty array
- **WHEN** an array with zero elements is encoded
- **THEN** the result is exactly `*0\r\n`

### Requirement: Reader and writer report errors as results
The reader SHALL expose a read operation returning a result that is either a parsed value or an error; the writer SHALL expose write operations returning a result of bytes written. The reader SHALL report an end-of-stream condition as an `eof` error and malformed or out-of-range protocol input as a `protocol_error` error. The reader SHALL NOT throw on malformed input, and integer/length parsing overflow SHALL be reported as `protocol_error`, never as an uncaught standard-library exception.

#### Scenario: End of stream
- **WHEN** the peer closes the connection and no further data arrives
- **THEN** the read result is an `eof` error that the caller can handle without an error reply

#### Scenario: Malformed type prefix
- **WHEN** a line does not start with a valid RESP2 type prefix
- **THEN** the read result is a `protocol_error`

#### Scenario: Non-numeric length
- **WHEN** a bulk-string or array header declares a non-numeric length
- **THEN** the read result is a `protocol_error`

#### Scenario: Integer overflow
- **WHEN** an integer or length value exceeds the representable range
- **THEN** the read result is a `protocol_error`

### Requirement: Command requests are correctly framed
The system SHALL encode command requests as arrays of bulk strings using the same encoder as replies, so every command element carries its own `$<len>` framing.

#### Scenario: GET request
- **WHEN** a GET command for key `key` is encoded
- **THEN** the result is exactly `*2\r\n$3\r\nGET\r\n$3\r\nkey\r\n`

#### Scenario: SET request with options
- **WHEN** a SET command for key `key` with value `val` and option `EX 10` is encoded
- **THEN** every element, including options, is framed as a bulk string