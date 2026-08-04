## Purpose

Defines how the client executes commands over the RESP2 protocol: building correctly framed requests, transmitting them, and returning the parsed server reply.

## ADDED Requirements

### Requirement: Command round-trip
The client SHALL execute a command by building a correctly framed RESP2 request from the command name and arguments, writing it to the connection, reading the reply, and returning the parsed reply as a typed RESP2 value.

#### Scenario: GET performs a full round-trip
- **WHEN** the client executes GET for a key
- **THEN** it sends the framed request `*2\r\n$3\r\nGET\r\n$3\r\n<key>\r\n`, reads the server reply, and returns the reply as a typed value

#### Scenario: Server error reply is propagated
- **WHEN** the server replies to a command with an error
- **THEN** the client returns the error as the result of the command

### Requirement: Connection failure is surfaced
The client SHALL surface connection-level failures (write or read errors, timeouts) as errors rather than returning a fabricated value.

#### Scenario: Write failure
- **WHEN** the connection cannot transmit the request
- **THEN** the client reports the write error and does not attempt to read a reply

## MODIFIED Requirements

None.

## REMOVED Requirements

None.
