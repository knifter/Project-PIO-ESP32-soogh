import subprocess
import datetime
from sys import stderr

#(part of) GIT revision
try:
    revision = (
        subprocess.check_output(["git", "rev-parse", "HEAD"])
        .strip()
        .decode("utf-8")
    )[-8:]
except subprocess.CalledProcessError:
    revision = "unknownrevision"
    print("Warning: Could not get git commit", file=stderr)

try:
    branch = (
        subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
        .strip()
        .decode("utf-8")
    )
except subprocess.CalledProcessError:
    print("Warning: Could not get git branch", file=stderr)
    branch = "unknownbranch"
    

#date
curr_date = datetime.datetime.now()
datetime = "%02d%02d%02d" % (curr_date.year, curr_date.month, curr_date.day)

print(f'-DBUILD_DATETIME="{datetime}"');
print(f'-DGIT_REVISION="{revision}"');
print(f'-DGIT_BRANCH="{branch}"');
