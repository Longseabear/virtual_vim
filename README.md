# Virtual VIM

I like to annotate my code. However, in the field, it is difficult to modify the code due to the version control system. To solve this, virtual vim builds a virtual file system in a specific folder and stores a copy there.

Put it in /usr/bin, and use it by registering the processing file as sudo through alias.

In the future (probably soon), we plan to implement the ddif2 algorithm and expand it so that it can respond even when the contents are modified.

# how to use
vvim filename

How it works: Internally, a virtual file directory will be created in the "/usr/local/vvim_dir/" path. And then, The file is copied as is.
And just system call "vim virtual_file_path"
