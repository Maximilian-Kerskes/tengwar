# Current Parsing Implementation Details for the Shell

## Parsing implementation details

- The parser uses a recursive descent parser to parse the shell input.

## Current parsing grammar

- list := pipeline (';' pipeline)\*
- pipeline := command ('|' command)\*
- command := ( WORD | redirection)\*
- redirection := '<' WORD | '>' WORD | '>>' WORD
