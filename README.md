# Jean

Jean is a software that statistics your programming habits.\
For general purposes, Jean can count the number of lines of code and comment information in different languages within the project.\

## Example

```text
/------------------------------------------------\
> [Language Top3]
> C++ (99.3%) | MakeFile (0.3%) | Markdown (0.2%)
- + + + + + + + + + + + + + + + + + + + + + + + +
> [Comment analysis]
> Comments (17%) | Blanks (3%) | Code (80%)
- + + + + + + + + + + + + + + + + + + + + + + + +
> [Indent analysis]
> 2-SPACE (97%) | 4-SPACE (1%) | TAB (X)
\------------------------------------------------/
Press [J] to check detail...
```
(under dev)

## How to use

### subdir (Default)

`$ jean`

### input local dir

`$ jean --target /path/to/dir`

### remote repository (support GitHub)

`$ jean --target github.com/iiran/jean`

## Install

### Dependencies
- boost
- curlpp
- nlohmann-json