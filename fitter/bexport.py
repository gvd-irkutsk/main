import argparse
import os
import subprocess

# feeds bexport-mc with packs of files

parser = argparse.ArgumentParser(description='Process multiple input files using bexport-mc.')

parser.add_argument('--in', dest='input_dir', required=True, help='Path to directory containing input .wout files')
parser.add_argument('--out', dest='output_dir', required=True, help='Path to directory for output .root files')
args = parser.parse_args()

# Construct the full path to the bexport-mc executable
barssys_path = os.environ.get('BARSSYS')
bexport_mc_path = os.path.join(barssys_path, 'bin', 'bexport-mc')

# Get a list of all the input files in the input directory
input_files = [f for f in os.listdir(args.input_dir) if f.endswith('.wout')]

# Process each input file
for input_file in input_files:
    # Construct the full path to the input file
    input_file_path = os.path.join(args.input_dir, input_file)
    # Construct the full path to the output file
    output_file = input_file.replace('.wout', '.root')
    output_file_path = os.path.join(args.output_dir, output_file)    
    # Run bexport-mc on the input file and redirect the output to the output file
    subprocess.run([bexport_mc_path, '--in', input_file_path, '--out', output_file_path], check=True)
