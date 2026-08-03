## Context

RESP reader/writer live in common/protocol. Current implementation assumes simple bulk-strings and arrays of strings. Several gaps observed: bulk nulls aren't detected, arrays treated as string-only, and malformed inputs may cause undefined behavior. Integer parsing appears correct but will be validated by tests; changes must be backward-compatible for existing convenience functions. Changes must be backward-compatible for existing convenience functions.

## Goals / Non-Goals

**Goals:**
- Provide complete RESP2 parsing/serialization semantics
- Keep public convenience API backward-compatible
- Add comprehensive tests (unit + round-trip + fuzz)

**Non-Goals:**
- Implement RESP3
- Change higher-level command semantics

## Decisions

1. Representation
- Use a variant type RESPValue = std::variant<std::monostate, std::int64_t, std::string, std::vector<RESPValue>, ErrorType> where std::monostate represents null bulk ($-1). This allows nested arrays and nulls.

2. Reader
- Implement recursive descent for arrays with depth counter and configurable max_depth.
- read_line() verifies CRLF ending and returns the line without CRLF.
- read_bulk_string recognizes "$-1" and returns std::monostate.
- read_integer uses std::stoll on the substring after ':' and handles leading '-'.
- Parse errors throw a specific ParseError exception type.

3. Writer
- Writer serializes RESPValue variants; nulls as $-1, nested arrays recursively, integers with ':' prefix, etc.
- Provide convenience overloads write_array(vector<string>) and write_simple_string for backward compatibility; implement them via the new writer.

4. Limits
- Add configuration constants in common/protocol/config.h: MAX_BULK_SIZE (e.g., 16MB default), MAX_ARRAY_DEPTH (e.g., 128).

## Risks / Trade-offs

- Adding std::variant with nested vectors increases type complexity across code that consumes reader output. Mitigation: keep convenience wrappers and introduce migration notes in design.
- Tests must be comprehensive; add CI job if flaky.

## Migration Plan

- Implement types and reader/writer changes behind internal API.
- Add tests and run local test suite.
- Replace direct uses in server/client connection_handler usage where currently parsing assumed vector<string>.

## Open Questions

- Should ParseError be a subclass of std::runtime_error or a project-specific error type? (Recommendation: project-specific with code enum.)
