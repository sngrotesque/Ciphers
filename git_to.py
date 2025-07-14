import os
import re
import sys
import subprocess
from pathlib import Path
from git.repo import Repo

# === 配置参数 ===
DEFAULT_FOLDER = Path('.')
REMOTE_URL     = 'git@github.com:sngrotesque/Ciphers.git'
COMMIT_MSG     = "General-Purpose Cryptographic Algorithms"
TARGET_VERSION = 'master'
EXCLUDE_DIRS   = ('.git')

NO_ = 'Algorithms/_compiled'

# === 工具函数 ===
def run_popen(cmd: str) -> str:
    result = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    return result.stdout.strip()

def run_command(cmd: str) -> int:
    return subprocess.call(cmd, shell=True)

# === Git 操作类 ===
class GitProcess:
    def __init__(self, folder: Path = DEFAULT_FOLDER):
        self.folder = folder.resolve()
        self.repo = Repo.init(self.folder)
        self.remote_url = REMOTE_URL
        self.commit_msg = COMMIT_MSG
        self.version = TARGET_VERSION
        self.files_to_add = self._get_files_to_add()

    def _get_files_to_add(self):
        return [f for f in os.listdir(self.folder) if f not in EXCLUDE_DIRS]

    def view_files(self):
        print(f'>>>> +---------- 当前文件列表(BEGIN) ----------+')
        for name in self.files_to_add:
            print(f'>>>> + {name:<35} +')
        print(f'>>>> +---------- 当前文件列表(END)   ----------+')

    def ensure_remote(self):
        try:
            self.repo.create_remote('origin', self.remote_url)
        except:
            pass  # 远程已存在

    def checkout_version(self):
        branches = run_popen('git branch')
        version_patterns = [
            r'\s+(v[\d.]+\-\w+)',
            r'\s+(v[\d.]+_\w+)',
            r'\s+(v[\d.]+)',
            r'\s+(\w+_v[\d.]+)'
        ]
        all_versions = []
        for pattern in version_patterns:
            all_versions += re.findall(pattern, branches)

        if self.version not in all_versions:
            run_command(f'git checkout -b {self.version}')
        else:
            run_command(f'git checkout {self.version}')

    def stage_files(self):
        for file in self.files_to_add:
            self.repo.index.add([file])

    def commit_and_push(self):
        self.repo.index.commit(self.commit_msg)
        self.repo.remote().push(self.version)

# === 主执行逻辑 ===
def main():
    if len(sys.argv) > 1 and sys.argv[1].lower() in {'yes', 'y'}:
        run_command('git rm --cached -r *')
    else:
        print('未指定是否清空 Git 缓存，默认保留。')

    git = GitProcess()
    git.ensure_remote()
    git.checkout_version()
    git.view_files()
    git.stage_files()
    git.commit_and_push()

if __name__ == '__main__':
    main()
