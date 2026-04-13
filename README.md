# strato

Strato is a commandline text editor, named after the stratosphere because it serves as the first step into the endless universe of commandline editors. It is intended as a stepping stone - the aim is:
- being easy to get started for folks new to commandline editors
- making it easy to get familiar with stratos command grammer to go from novice to experienced
- making users familiar enough with modal editing to provide a smooth transition to editors with more complex grammars, such as vim or helix

his readme is written for version 0.6.6.

## ARCHITECTURE

The under-the-hood architecture is built around a Model-View-Controller architecture, with a state machine for modal editing and mode-specific shortcuts. User Actions are processed using a command pattern, allowing for a low-cost undo/redo history via delta tracking.

## KEY FEATURES

strato displays useful meta information about the file currently being edited:
- the current mode
- The file currently being edited
- the save state of the file
- the cursor position
- the number of words, paragraphs and characters
- the current version of the editor
- paragraph numbers along the left edge

There are also several quality-of-life features to make using strato as smooth as possible. The text history system groups text into words when undoing them to make deleting larger sections of text or undoing large deletions faster. Additionally files are periodically backed up to a fixed location relative to the location of the executable. That way, you never lose more than five minuts of progress.

> As of version 0.6.3, strato supports highlighting for headings, quote blocks, bold and italic in markdown files

### USER CUSTOMIZATION

As of version 0.6.6, strato can provide simple, token-based syntax highlighting in markdown code blocks and in files with extensions matching the code language. Some popular languages are already added, but users can add new languages at any time by adding a new file following the provided template.

Color theme customization will be added in a future update.

## GETTING STARTED

### MINIMUM REQUIRED KNOWLEDGE

To get started with strato, you will need to understand the two core modes: TYPING MODE is used for writing and deleting text, whereas TOOL MODE is used for all of stratos commands. When in TOOL MODE, press 'i' to enter TYPING MODE. When in TYPING MODE, press the escape key to enter TOOL MODE. Move the cursor with the arrow keys in either mode or, if you are in TOOL MODE, using h (left), j (down), k (up) and l (right). When in TOOL MODE, you can save with !s and quit with !q.

### EXISTING COMMANDS
Unless otherwise specified, all commands stay in TOOL MODE.

The following commands require only a single input:

- e & E: the ERASE operator. Erases the character under the cursor. E (uppercase) enters TYPING MODE, e (lowercase) stays in TOOL MODE
- u & U: the UNDO & REDO operator. u (lowercase) undoes the most recent editing action, U (uppercase) redoes the most recently undone action.
- o & O: the paragraph creation operator. o (lowercase) creates a paragraph below the current one, O (uppercase) creates a paragraph above the current one
- + & -: the Split & Join operators. + (plus) joins the next paragraph to the current one, - (minus) splits the current paragraph from the cursor onwards into its own paragraph
- < & >: the INDENTATION operator. > (greater than) indents the current paragraph to the next multiple of four spaces, < (less than) unindents the current paragraph to the previous multiple of four spaces. TAB and SHIFT-TAB do the same thing, both in TYPING MODE and in TOOL MODE. Currently, tab width is locked to four spaces, but it will be possible to configure this in a future update.

The next section describes 'compound commands', which are commands where a secondary input is needed. There are three types of input: Scopes, ranges and arguments.

Scopes divide the text to specify what text area will be affected by commands. At any point, the cursor is in all five scopes at once - think of them as subsets of each other.
- w = word: one or more letters and numbers, seperated by whitespace or special characters. (or the end of a paragraph)
- e = expression: one or more letters, numbers and special characters, seperated by whitespace (or the end of a paragraph)
- l = line: a visual line on the screen (depends on window width)
- p = paragraph: one paragraph of text
- f = file: the entire file

Ranges are sections of text surrounded by a matching pair of delimiting characters:
- " = within double quotes
- ' = within single quotes
- ( = within parentheses
- { = within curly braces
- [ = within brackets
- < = within angled brackets
Ranges account for nesting, meaning in a string like '(abc(def)ghi)', with the cursor on 'a', 'm(' (move to the end of the parentheses) would move to i, not f, because 'def' is within nested parentheses.

Arguments are a general-purpose type of input that will be processed individually by each operator.

MOVE operator (m & M): Accepts either a scope or a range. m (lowercase) moves the cursor forward to the end of the given scope or range. M (uppercase) moves the cursor backward to the start of the given scope or range.

NEXT operator (n & N): Accepts either a scope or a range. n (lowercase) moves the cursor to the start of the next scope or range. N (uppercase) moves the cursor backwards to the end of the previous scope or range. There are also some aliases for this operator, as it is essential for fast movement: H and L alias 'Nw' and 'nw', and J and K alias 'Np' and 'np'.

FIND operator (f & F): Accepts an argument. f (lowercase) moves the cursor to the next occurrence of that argument. F (uppercase) moves the cursor backwards to the previous occurrence of that argument. As opposed to vim, this is not limited to the current paragraph.

AT operator (a & A): Accepts either a scope or a range. Moves the cursor just like the move operator, but also enters TYPING MODE.

> NOTE: In case the target position is not reachable (e.g. f; without a semicolon to find, nf,...), all movement operations described above move as far as possible.

REPLACE operator (r): Accepts an argument. The character under the cursor is replaced by the argument.

TOGGLE CASE operator (t & T): Accepts either a scope or a range. t (lowercase) sets that scope or range to lowercase, T (uppercase) sets the scope or range to uppercase.

SYSTEM operator (!): Accepts one of the following arguments:
- q: if all changes are saved, quit. Otherwise display a warning.
- Q: save all changes and quit.
- x: quit without saving
- s: save

DELETE operators (d & D): d (lowercase) accepts a scope or range. That area is deleted. D (uppercase) accepts an argument. The area from the cursor to the next occurence of the argument is deleted (including the argument and the starting position).

CHANGE operators (c & C): c and C work just like d and D, but switches to TYPING MODE.

YANK operators (y & Y): y (lowercase) accepts a scope or range. That area is copied to an internal clipboard. Y (uppercase) accepts an argument. The area from the cursor to the next occurence of the argument is copied to an internal clipboard (including the argument and the starting position).

PASTE operator (p & P): p (lowercase) pastes the contents of the internal clipboard after the cursor, P (uppercase) pastess the contents of the internal clipboard before the cursor.

REPEAT operator (.): Repeats the last editing command (specifics to be determined)

### UPCOMING OPERATORS

HELP operator (?): Accepts an argument. If that argument is an operator, usage tips for that operator are displayed as a temporary message. If that argument is also a scope or range indicator, that scope or range is explained as well.

SYSTEM operator (!): In addition to the already existing options, these will be added:
- r: rename the file
- c: configure settings

## Technical Stack

Strato is written in C++ 20 using the ncurses library for rendering and input handeling. Everything else was built from scratch or uses the C++ standard library.

## ROADMAP & LIMITATIONS
What can it do, what can't it do, what will it be able to do soon

**PLANNED FEATURES**
1. Simple spell checking system
2. a built-in help menu for easy learing of shortcuts
3. selection mode similar to vim's visual mode
4. changing settings (e.g. line numbering, meta information display, UI color) at runtime
5. Some level of ascii table support (details tbd)
6. switching between multiple files
7. hot loading if an opened file is changed by an external source (details tbd)
8. Find and Search / Replace commands
9. changing UI color


**LIMITATIONS THAT WILL EXIST FOR THE FORSEEABLE FUTURE**
- no unicode support (ascii only)
- no interaction with system clipboard 
- no rendering of images (on account of this being a command line editor)