
import os

file_path = r"c:\Projetos\wave1.8\WavePwn\data\web\index.html"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1: Mascot onchange
old_mascot = 'onchange="MascotSystem.setMascot(this.value)"'
new_mascot = 'onchange="MascotSystem.setMascot(this.value); updateMascotIcons(this.value)"'
if old_mascot in content:
    content = content.replace(old_mascot, new_mascot)
    print("Fixed mascot onchange.")
else:
    print("Warning: Mascot onchange string not found.")

# Fix 2: Replace inline script with external file ref
# Identify the start of the block
start_marker = '<script>\n        // Additional functions for new features'
# Identify the end
end_marker = '</script>\n</body>'

start_idx = content.find(start_marker)
end_idx = content.rfind(end_marker)

if start_idx != -1 and end_idx != -1:
    # We want to keep the end_marker's </body> tag, but replace the </script> with the new tag?
    # No, we want to replace the whole <script>... content ... </script> block.
    # The end_marker includes </script>.
    # So we replace from start_idx to end_idx + len('</script>')
    
    # Wait, end_marker is `</script>\n</body>`. rfind finds the start of this string.
    # So we want to replace up to the `\n` before `</body>`?
    # Let's be precise.
    
    # Original:
    # <script>
    # ...
    # </script>
    # </body>
    
    # New:
    # <script src="js/attacks.js"></script>
    # </body>
    
    # So we replace everything from start_idx to end_idx + len('</script>')
    # with '<script src="js/attacks.js"></script>'
    
    script_end_tag_len = len('</script>')
    # verifies that end_idx points to the </script> before body
    
    # Perform replacement
    # Re-slice
    new_content = content[:start_idx] + '<script src="js/attacks.js"></script>' + content[end_idx + script_end_tag_len:]
    content = new_content
    print("Replaced inline script with js/attacks.js reference.")
    
else:
    print(f"Warning: Script block markers not found. Start: {start_idx}, End: {end_idx}")

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)
