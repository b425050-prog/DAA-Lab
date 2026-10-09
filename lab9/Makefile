PYTHON ?= python3
.PHONY: all strict test evidence visuals clean
all strict:
	$(PYTHON) tools/build.py
test: all
	$(PYTHON) tools/validate.py
evidence: test
	$(PYTHON) tools/generate_evidence.py
visuals: evidence
	$(PYTHON) tools/generate_visuals.py
clean:
	$(PYTHON) -c "from pathlib import Path; import shutil; p=Path('bin').resolve(); assert p.parent == Path.cwd().resolve(); shutil.rmtree(p) if p.exists() else None"
