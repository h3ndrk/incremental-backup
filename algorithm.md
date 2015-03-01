# Algorithm

1. open index
2. update all files and set flag to 1
3. walk through directory for every file:
    1. update flag of the file to 0
    2. file in index is older:
        * store file in archive
        * update timestamp of the file
4. delete all files where flag is set to 1 (clean up index)
5. close index