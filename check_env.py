# save as check_env.py
import sys

pkgs = [
    ("NumPy",       "numpy"),
    ("SciPy",       "scipy"),
    ("pandas",      "pandas"),
    ("matplotlib",  "matplotlib"),
    ("scikit-learn","sklearn"),
    ("PyTorch",     "torch"),
    ("TorchVision","torchvision"),
    ("TorchAudio", "torchaudio"),
]

errors = []
for name, mod in pkgs:
    try:
        m = __import__(mod)
        v = getattr(m, "__version__", "unknown")
        print(f"{name:12s} version: {v}")
    except Exception as e:
        errors.append(f"{name}: import error -> {e}")

if errors:
    print("\n--- Import Errors ---")
    print("\n".join(errors))
    sys.exit(1)
else:
    print("\nAll imports succeeded without errors.")