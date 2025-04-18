import re

def load_new_values(mapping_file):
    """Load new parameter values from the mapping file."""
    new_values = {}
    with open(mapping_file, 'r') as f:
        for line in f:
            parts = line.strip().split(',')
            if len(parts) == 2:
                param, value = parts[0].strip(), parts[1].strip()
                new_values[param] = value
    return new_values

def update_tune_params(input_file, output_file, mapping_file):
    """Update the first number argument in TUNE_PARAM or NO_TUNE_PARAM based on the mapping."""
    new_values = load_new_values(mapping_file)

    # allow optional + or - in the old value
    pattern = re.compile(
        r'^(.*?\b(TUNE_PARAM|NO_TUNE_PARAM)\((\w+),\s*)'  # prefix up to first value
        r'([+-]?\d+)'                                     # capture signed integer
        r'(,.*)$'                                         # rest of the line
    )

    with open(input_file, 'r') as f:
        lines = f.readlines()

    with open(output_file, 'w') as f_out:
        for line in lines:
            m = pattern.match(line)
            if m:
                prefix, _, param_name, old_value, suffix = m.groups()
                if param_name in new_values:
                    new_val = new_values[param_name]
                    line = f"{prefix}{new_val}{suffix}\n"
            f_out.write(line)

# Example usage
update_tune_params('original.txt', 'output.txt', 'spsa_out.txt')
