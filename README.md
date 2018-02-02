serverX
================

[![License: MIT](https://img.shields.io/badge/License-Apache-blue.svg)](https://raw.githubusercontent.com/BytesClub/serverX/master/LICENSE)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/73b7308c3101417d81e09a9dac1d4032)](https://www.codacy.com/app/BytesClub/serverX?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=BytesClub/serverX&amp;utm_campaign=Badge_Grade)
[![Issues](https://img.shields.io/github/issues/BytesClub/serverX.svg)](https://github.com/BytesClub/serverX/issues)
[![Pull Request](https://img.shields.io/github/issues-pr/BytesClub/serverX.svg)](https://github.com/BytesClub/serverX/pulls)
[![Releases](https://img.shields.io/github/tag/BytesClub/serverX.svg)](https://github.com/BytesClub/serverX/releases)
[![Mailing List](https://img.shields.io/badge/Mailing%20List-BytesClub-blue.svg)](mailto:bytes-club@googlegroups.com)
[![gitter](https://badges.gitter.im/gitterHQ/gitterHQ.github.io.svg)](https://gitter.im/Bytes_Club/General)

![serverX Logo](http://codeprogyan.me/img/portfolio/serverx.jpg)

A HTTP/HTTPS Web Server Handler by (C) Bytes Club

Installation
================

* Install dependencies and toolchain
```
$ apt-get update
$ apt-get install build-essential zlib1g-dev php7.0-cgi
```

* Get Source Code
```
$ apt-get install git
$ git clone https://github.com/BytesClub/serverX.git
$ cd serverX
```

* Build program _(Make sure current directory contains a Makefile)_
```
$ make
```

Testing
================

* Open terminal and go to Source directory
```
$ make test
```

* Open any browser and go to following address
```
http://localhost:8080
```

Contributing
================

* Reference: [Contributing Guidelines](.github/CONTRIBUTING.md)
  * Every commit must follow the following standard:

  ```
  Shortlog: Commit Message

  commit body

  Fixes: #<issue_number>
  [Closes: <In case of multiple issues>]

  Signed-off-by: [Your name] <your email>
  ```

* ### Contributing in GitHub:
  * Fork the project in your account and clone to your local machine.
  * Create a branch abiding following format:
  ```
  <Your-Name/Your-Username>/Issue-<IssueID>
  ```
  * Update with latest at **master** branch.
  ```
  $ git remote add upstream https://github.com/BytesClub/serverX.git
  $ git fetch upstream
  $ git checkout [-b] <your-branch>
  $ git rebase upstream/master --strategy recursive -X theirs
  $ git push origin <your-branch> [-u]
  ```
  * Make changes and commit them according to the guideline provided.
  * **Do not commit anything to the master branch.**
  * Create a Pull Request to the Base Repository.

* ### Contributing via Patches:
  * Install `git-email` package on top of `git-core` package already installed and configure accordingly.
  * Create a branch abiding following format:
  ```
  <Your-Name/Your-Username>/Issue-<IssueID>
  ```
  * Update with latest at **master** branch.
  ```
  $ git rebase origin/master --strategy recursive -X theirs
  ```
  * Make changes and commit them according to the guideline provided.
  * **Do not commit anything to the master branch.**
  * Format patch from the commits you've created.
  ```
  $ git format-patch <commit> -<N> --cc="Progyan Bhattacharya <progyanb@acm.org>" -o ../serverX-patches
  ```
  * Send email to `bytes-club@googlegroups.com` containing the patch.
  ```
  $ git send-email ../serverX-patches/<patch> --to="bytes-club@googlegroups.com" --subject="[Patch] BytesClub: serverX: <Short Patch Info>"
  ```

Legal Notice
================

    Copyright 2017 The serverX Authors
    Maintainer: Bytes Club (https://bytesclub.github.io)<bytes-club@googlegroups.com>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
