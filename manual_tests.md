# Manual Tests

This file contains manual test instructions for not yet automated tests:

## Console Window + Console Output

### console window tidy-up

For windows, a separate console window might be opened for logging.
We need to make sure that closing console or Qt UI triggers closinfg the other one

test cases:

- visual studio:
   - start via play button:
      - test: a console and a gui window appear (2 windows!)
      - close gui window
      - test: the console window closes automatically

- visual studio:
   - start via play button:
      - test: a console and a gui window appear (2 windows!)
      - close console window
      - test: the gui window closes automatically
      - 

- visual studio:
   - start via play button:
      - test: a console and a gui window appear (2 windows!)
      - hit Ctrl+C in console window
      - test: both windows close automatically


