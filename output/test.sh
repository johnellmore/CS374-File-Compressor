#!/bin/bash

echo ""
echo "Running numbered tests. No test should generate output."




# TEXT FILES
echo ""
echo "Testing single text file compression..."

# test 1
echo "    Running test 1..."
../squash ../tests/text/1-empty.txt -a lzw -o 1-empty.cmp
../squash -e 1-empty.cmp
diff 1-empty.txt ../tests/text/1-empty.txt

# test 2
echo "    Running test 2..."
../squash ../tests/text/2-stringWithNull.txt -a lzw -o 2-stringWithNull.cmp
../squash -e 2-stringWithNull.cmp
diff 2-stringWithNull.txt ../tests/text/2-stringWithNull.txt

# test 3
echo "    Running test 3..."
../squash ../tests/text/3-simple.txt -a lzw -o 3-simple.cmp
../squash -e 3-simple.cmp
diff 3-simple.txt ../tests/text/3-simple.txt

# test 4
echo "    Running test 4..."
../squash ../tests/text/4-LZWspecial.txt -a lzw -o 4-LZWspecial.cmp
../squash -e 4-LZWspecial.cmp
diff 4-LZWspecial.txt ../tests/text/4-LZWspecial.txt

# test 5
echo "    Running test 5..."
../squash ../tests/text/5-declaration.txt -a lzw -o 5-declaration.cmp
../squash -e 5-declaration.cmp
diff 5-declaration.txt ../tests/text/5-declaration.txt

# test 6
echo "    Running test 6..."
../squash ../tests/text/6-constitution.txt -a lzw -o 6-constitution.cmp
../squash -e 6-constitution.cmp
diff 6-constitution.txt ../tests/text/6-constitution.txt

# test 7
echo "    Running test 7..."
../squash ../tests/text/7-hammurabi.txt -a lzw -o 7-hammurabi.cmp
../squash -e 7-hammurabi.cmp
diff 7-hammurabi.txt ../tests/text/7-hammurabi.txt




# IMAGE FILES
echo ""
echo "Testing binary files (images)..."

# test 8
echo "    Running test 8..."
../squash ../tests/images/8-imageblank.gif -a lzw -o 8-imageblank.cmp
../squash -e 8-imageblank.cmp
diff 8-imageblank.gif ../tests/images/8-imageblank.gif

# test 9
echo "    Running test 9..."
../squash ../tests/images/9-animated.gif -a lzw -o 9-animated.cmp
../squash -e 9-animated.cmp
diff 9-animated.gif ../tests/images/9-animated.gif

# test 8
echo "    Running test 10..."
../squash ../tests/images/10-compleximage.png -a lzw -o 10-compleximage.cmp
../squash -e 10-compleximage.cmp
diff 10-compleximage.png ../tests/images/10-compleximage.png




# WHOLE DIRECTORIES
echo ""
echo "Testing nested directories..."

# test 8
echo "    Running test 11..."
../squash ../tests -a lzw -o 11-directory.cmp
../squash -e 11-directory.cmp
diff -r tests ../tests




# MULTIPLE ENTRIES
echo ""
echo "Testing multiple entries..."

# test 8
echo "    Running test 12..."
../squash ../tests/text ../tests/images ../tests/text/7-hammurabi.txt -a lzw -o 12-multiple.cmp
../squash -e 12-multiple.cmp
diff -r tests/text ../tests/text
diff -r tests/images ../tests/images
diff tests/text/7-hammurabi.txt ../tests/text/7-hammurabi.txt




# DONE
echo "Finished running tests."