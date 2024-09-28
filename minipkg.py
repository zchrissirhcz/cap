import os
import shutil

class PreparationError(Exception):
    pass

class PortableFileDialogsPkg:
    def __init__(self):
        self.git_remote = 'https://github.com/samhocevar/portable-file-dialogs'
        self.exptected_git_sha = '7f852d88a480020d7f91957cbcefe514fc95000c'
        self.src_dir = os.path.expanduser('~/work/github/portable-file-dialogs')
        self.dst_dir = os.path.expanduser('~/.minipkg/portable-file-dialogs')

    def prepare(self):
        # Ensure git is installed
        if os.system('which git') != 0:
            raise PreparationError('Git is not installed')
        
        # Ensure source directory exists
        if not os.path.exists(self.src_dir):
            os.makedirs(self.src_dir, exist_ok=True)
            cmd = 'git clone {} {}'.format(self.git_remote, self.src_dir)
            if os.system(cmd) != 0:
                raise PreparationError('Failed to clone git repository')

        # Ensure source directory is a git repository
        if os.system('git -C {} rev-parse'.format(self.src_dir)) != 0:
            raise PreparationError('Directory is not a git repository')

        # Ensure directory is without uncommitted changes
        cmd = 'git -C {} diff --exit-code'.format(self.src_dir)
        if os.system(cmd) != 0:
            raise PreparationError('Directory has uncommitted changes')

        # Check git sha
        # checkout the expected git sha, if failed raise an error
        cmd = 'git -C {} checkout {}'.format(self.src_dir, self.expected_git_sha)
        if os.system(cmd) != 0:
            raise PreparationError(f'Failed to checkout git sha: {self.expected_git_sha}')

        return True

    def install(self):
        # Copy file
        src_pth = f'{self.src_dir}/portable-file-dialogs.h'
        dst_pth = f'{self.dst_dir}/portable-file-dialogs.h'

        os.makedirs(self.dst_dir, exist_ok=True)
        shutil.copyfile(src_pth, dst_pth)

        print(f'Installed {src_pth} to {dst_pth}')


if __name__ == '__main__':
    pkg = PortableFileDialogsPkg()
    try:
        pkg.prepare()
        pkg.install()
    except PreparationError as e:
        print(e)
        exit(1)