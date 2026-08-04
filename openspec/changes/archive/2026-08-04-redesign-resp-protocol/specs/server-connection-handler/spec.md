## Purpose

Defines how the server consumes the RESP2 protocol: reading requests, dispatching them to command handlers, handling protocol errors and client disconnects, and replying with protocol-compliant errors.

## ADDED Requirements

### Requirement: Request dispatch
The server SHALL read incoming requests as RESP2 values and dispatch them to command handlers. An array request SHALL be dispatched with its items as command arguments (item 0 is the command name); a single bulk-string request SHALL be dispatched as a one-argument request. Requests of any other type (simple string, integer, error) SHALL be answered with an `-ERR` reply.

#### Scenario: Array request is dispatched
- **WHEN** the server reads an array whose first item names a supported command
- **THEN** it invokes the command handler with the command name and remaining items as arguments

#### Scenario: Single bulk string request is dispatched
- **WHEN** the server reads a request consisting of a single bulk string
- **THEN** it dispatches it as a command with no additional arguments

#### Scenario: Unsupported request type
- **WHEN** the server reads a simple string, integer, or error as a request
- **THEN** it replies with an error describing the request as unsupported

### Requirement: Unknown command reply
The server SHALL reply with an `-ERR` response when a request names a command that has no registered handler.

#### Scenario: Unregistered command
- **WHEN** the server receives an array request whose command name has no handler registered
- **THEN** it replies with an `-ERR` response indicating the command is not supported

### Requirement: Client disconnect ends the session
The server SHALL end a connection's request loop when the client disconnects, without sending an error reply for the disconnection itself.

#### Scenario: Peer closes connection
- **WHEN** the client closes the connection after a completed request
- **THEN** the server ends the session for that connection without sending an error reply

### Requirement: Protocol error reply and close
The server SHALL reply with `-ERR` describing a protocol error and then close the connection when it reads malformed protocol input.

#### Scenario: Malformed request
- **WHEN** the server reads bytes that do not form a valid RESP2 request
- **THEN** it replies with an error describing the protocol error and closes the connection

## MODIFIED Requirements

None.

## REMOVED Requirements

None.
