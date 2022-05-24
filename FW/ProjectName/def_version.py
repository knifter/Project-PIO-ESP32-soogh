import subprocess
import datetime

#(part of) GIT revision
revision = (
    subprocess.check_output(["git", "rev-parse", "HEAD"])
    .strip()
    .decode("utf-8")
)
print(f'-DGIT_REVISION="{revision[-8:]}"');

branch = (
    subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    .strip()
    .decode("utf-8")
)
print(f'-DGIT_BRANCH="{branch}"');

#date
curr_date = datetime.datetime.now()
datetime = "%02d%02d%02d" % (curr_date.year, curr_date.month, curr_date.day)
print(f'-DBUILD_DATETIME="{datetime}"');
