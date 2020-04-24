# byteticker

Byteticker is a simple utility to stream bytes from stdin to stdout while reporting the byte count and throughput.  It takes as the
only argument the buffer size, either numerical or with a case-insensitive abbreviation such as "k", "m", or "g", for KiBytes,
MiBytes, or GiBytes, respectively.

The executable name is "btr".

The output will look like:

```
$ cat /dev/zero | btr 1g >/dev/null
       6,373,638,144 bytes ++    3 GB/s
```

That's it.  Enjoy.
