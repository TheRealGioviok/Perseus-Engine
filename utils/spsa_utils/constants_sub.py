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
    """Update the first number argument in TUNE_PARAM or NO_TUNE_PARAM based on the mapping, preserving comments."""
    new_values = load_new_values(mapping_file)
    pattern = re.compile(r'^(.*?\b(TUNE_PARAM|NO_TUNE_PARAM)\((\w+),\s*)(\d+)(,.*)$')
    
    with open(input_file, 'r') as f:
        lines = f.readlines()
    
    updated_lines = []
    
    for line in lines:
        match = pattern.match(line)
        if match:
            prefix, param_type, param_name, old_value, suffix = match.groups()
            if param_name in new_values:
                updated_line = f"{prefix}{new_values[param_name]}{suffix}\n"
            else:
                updated_line = line
        else:
            updated_line = line
        
        updated_lines.append(updated_line)
    
    with open(output_file, 'w') as f:
        f.writelines(updated_lines)

# Example usage
update_tune_params('original.txt', 'output.txt', 'spsa_out.txt')
