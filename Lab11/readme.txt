To Run the code:-
------------------------------------------------------------

- Download the bbfs files
- Run the .configure script
- Replace src/Makefile with the provided makefile
- Replace src/bbfs.c with the provided makefile
- In the src directory, run make

- Now change to the example directory
- There are two encryption options available.
- By default the executable runs using a simple cypher.
- Running "../src/bbfs rootdir/ mountdir/" invokes this option.
- Using the --useSSL flag makes the program use AES128 encryption.
- For this run "../src/bbfs --useSSL rootdir/ mountdir/"
- This option is buggy. Initial block writes to files works perfectly but further appending does not work correctly.


Structure of the code:-
------------------------------------------------------------

BBFS implements a variety of functions that replace the ordinary file system calls of the system. When a call is made to a system call within the mount directory, the corresponding bbfs function is called.
The mapping between user calls and defined functions is available in a struct that is given as an argument to fuse. From this struct we find that read and write correspond to bb_read and bb_write respectively.
Within bb_read a call to read is made to populate the input buffer with data from the file from the root directory. This is the encrypted data. We insert the code to decrypt the buffer and make it readable before we return from the bb_read. Similarly in bb_write we encrypt the given buffer and store it in a new buffer. This encrypted buffer is what we write to the file.

The problem with AES:-
------------------------------------------------------------

AES encrypts the text in blocks of size 16 bytes. If the text inserted initially is not a multiple of this lenght some encryption bits are left out. Inserting text after this point messes up the structure and the text beyond this point cannot be decrypted correctly. We can solve this problem using suitable padding after the insertion to maintain the file in blocks of 16 bytes. We have not been able to make this work correctlyyet.

Testing:-
-----------------------------------------------------------

We can echo text to a file in the mount directory.
Then printing out the file with cat  in the root directory reveals the encrypted text.
Using cat in the mount directory reveals the original text.

