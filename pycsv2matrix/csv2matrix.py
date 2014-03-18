#!/usr/bin/python2
# -*- coding: utf-8 -*-

import pdb
import sys
import argparse
import json

class csv2matrix:

    def __init__(self, argv):
        # Parse commandline arguments
        parser = argparse.ArgumentParser(description='Convert a csv file to a matrix')
        parser.add_argument('-n', '--no_header', dest="no_header", action='store_false',
                            help='should be provided if the csv file does not provide header information')
        parser.add_argument('-l', '--last_line', dest="last_line", action='store_true',
                            help='should be provided if the last line contains the column description')
        parser.add_argument('-c', dest='csv_split', default=';',
                            help='define csv separation token (default=";")')
        parser.add_argument('-m', dest='matrix_split', default=';',
                            help='define matrix column separation token (default=";")')
        parser.add_argument('-d', dest='matrix_columns', default=-1,
                            help='define maximum number of columns in the matrix, if there are less columns, empty values will be added, if there are more columns those will NOT be omitted.')
        parser.add_argument('-r', dest='raw_columns', default='',
                            help='Define the columns that will be copied without processing (i.e. columns with date information). This should be a space separated list of column numbers where the first column is 0 (zero).')
        parser.add_argument('-u', dest='unique_ids', action='store_true',
                            help='Should be provided if all ids representing text in the csv should be unique.')
        parser.add_argument('-i', '--input', dest='csv_file', default='',
                            help='define input csv file (required)')
        parser.add_argument('-o', '--output', dest='matrix_file', default='',
                            help='define the output matrix file (optional)')
        self.args = parser.parse_args(argv)

        if self.args.raw_columns:
            raw_columns = list()
            raw_list = self.args.raw_columns.split(" ")
            for value in raw_list:
                if self.is_number(value):
                    raw_columns.append(float(value))
                else:
                    parser.print_help()
                    print "Error: Raw columns invalid!"
                    sys.exit()
            self.args.raw_columns = raw_columns
        if not self.args.csv_file:
            parser.print_help()
            print "Error: No input file provided!"
            sys.exit()
        if not self.args.matrix_file:
            if self.args.csv_file.endswith(".csv"):
                self.args.matrix_file = self.args.csv_file[:-4] + ".matrix"
            else:
                self.args.matrix_file = self.args.csv_file + ".matrix"

        self.unique_id = 0

    def is_number(self, s):
        """ Check if the provided string is a number (representation)

        Returns True if the string is a number, else returns False.
        """
        try:
            float(s)
            return True
        except ValueError:
            return False

    def dictionary_value(self, dictionary, key):
        """ Provides a unique number for the provided value

        Checks if the dictionary contains the key. If it does not contain the key,
        an identifier is created, stored in the dictionary and returned, else the
        identifier is retrieved and returned.
        """
        if key in dictionary:
            return dictionary[key]
        else:
            number = len(dictionary)
            if self.args.unique_ids:
                number = self.unique_id
                self.unique_id += 1
            dictionary[key] = number
            return number

    def process_line(self, dictionary, key):
        """ Converts one line from csv to matrix
        """
        pass

    def convert(self):
        print "Creating matrix file " + self.args.matrix_file + " from " + self.args.csv_file
        # Read csv file per line
        line_count = 0
        column_count = 0
        prev_line = ""
        header = ""
        column_legends = list() # contains a dictionary if the column is not a number
        with open(self.args.csv_file, "r") as csv, open(self.args.matrix_file, "w") as matrix:
            for line in csv:
                # Skip empty lines
                if not line:
                    print " * Empty line at " + str(line_count)
                    continue

                # Process previous line
                if prev_line:
                    # Process previous line
                    column_list = prev_line.split(self.args.csv_split)
                    matrix_line = ""
                    index = 0
                    for value in column_list:
                        if index not in self.args.raw_columns:
                            # If we end up here, then this is not a raw column
                            if len(column_legends) >= index:
                                if self.is_number(value):
                                    column_legends.append(None)
                                else:
                                    column_legends.append(dict())
                            # Here we know that the column is defined in the legend
                            if column_legends[index] is not None:
                                value = self.dictionary_value(column_legends[index], value)
                        else:
                            if len(column_legends) >= index:
                                column_legends.append(None)
                        matrix_line = matrix_line + str(value) + self.args.matrix_split
                        index += 1
                    while (index < int(self.args.matrix_columns)):
                        matrix_line = matrix_line + self.args.matrix_split
                        index += 1
                    if self.args.matrix_columns > 0 and self.args.matrix_columns < index:
                        self.args.matrix_columns = index
                    matrix.write(matrix_line + "\n")

                # Process header
                if line_count == 0 and not self.args.last_line and not self.args.no_header:
                    # This line is the header
                    print " * Found header"
                    header = line
                    continue
                prev_line = line
                line_count += 1

        # Generate legend
        if self.args.last_line:
            print " * Found header"
            header = line
        # Provide converstion info and legend
        obj = dict()
        obj["arguments"] = self.args
        obj["header"] = header.split(self.args.csv_split)
        obj["legend"] = column_legends
        legend_body = json.dumps(obj)
        text_file = open(self.args.matrix_file + "_legend.json", "w")
        text_file.close()
        # Show output summary
        print "Matrix contains:"
        print header
        print "\trows:\t" + str(line_count - 1)
        print "\tcolumns:\t" + str(column_count)

if __name__ == '__main__':
    start = csv2matrix(sys.argv[1:])
    start.convert()
