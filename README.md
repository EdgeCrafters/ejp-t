# EJP-T: Edged Judger Platform for Teachers 🍎

EJP-T is a specialized component of the Edged Judger Platform (EJP) designed to empower educators in the realm of coding education. Seamlessly integrated with the EJP ecosystem, EJP-T enables teachers to craft assignments, manage student grades, and even automate the grading process.

## Table of Contents
- [Highlights 🌟](#highlights-🌟)
- [Getting Started 📚](#getting-started-📚)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Usage](#usage)
- [Contributing 🤝](#contributing-🤝)
- [License 📄](#license-📄)
- [Details](#details)
  - [manage](#manage)
    - [```[command]```](#command)
  - [workbook](#workbook)
    - [```[command]```](#command-1)


## Highlights 🌟

- **Craft Assignments**: Create coding assignments with custom criteria, deadlines, and point values.
- **Automated Grading**: Leverage automated grading scripts to evaluate student submissions.
- **Grade Management**: Keep track of student grades and progress with a simple and intuitive dashboard.
- **Cross-Component Integration**: Works in tandem with EJP-S (for Students) and EJP-BE (Server Program) to provide a comprehensive educational experience.

## Getting Started 📚

### Prerequisites

- Ensure that you have the main EJP system set up and running.
- Requires Linux OS (Windows & Mac support coming soon).

### Installation

1. Clone the EJP-T repository.
    ```bash
    git clone https://github.com/EdgeCrafters/ejp-t.git
    ```
2. Navigate to the cloned directory and run the installation script.
    ```bash
    cd ejp-t && chmod 755 scripts/setup.sh && chmod 755 scripts/install.sh
    ./scripts/setup.sh && ./scripts/install.sh
    source ~/.bashrc
    # source ~/.zshrc (if you use zsh, or whatever, customize this command)
    ```
3. Follow the on-screen prompts to complete the installation.

### Usage

1. Launch EJP-T from the command line:
    ```bash
    ejp-t [category] [command] [options] 
    ```
    Parameters
    * [category]: The category to which the command belongs (e.g., manage, workbook).
    * [command]: The specific action you want to perform (e.g., create, append, enroll).
    * [options]: Additional parameters or flags that modify the command. (e.g. -h(host), -l(location)).
    See [below](#Details) for more information about parameters.
2. Use the menu options to create new assignments, manage grades, or sync with EJP-S and EJP-BE.
3. For a list of all available commands and options, use:
    ```bash
    ejp-t help
    ```

## Contributing 🤝

We welcome contributions from the community! Feel free to fork the repository, make your changes, and submit a pull request. For more details, check out our [contribution guidelines](#).

## License 📄

EJP-T is part of the EJP project and follows the same MIT license.

## Details
* **Caching Options & Authentication**: Some `[options]` and authentication information can be saved locally to eliminate redundant input.
### manage
you can mange workbook(s).

``` bash
ejp-t manage [command] [options]
```
</br>

#### ```[command]```
0. ```create``` : create user(s) in certain host
    </br>
    * ```-h``` : url of server, the url of the server where the workbook, problem, or test cases are stored. this information can be cached locally for future use.
    * ```-u``` : a username to create
    * ```-p``` : password of a user to create
    * ```-l``` : path to `.csv` file with usernames and passwords (makes -u and -p optional)
    </br>

    ```bash
    ejp-t manage create -u john1234 -p neo666
    ```
    or
    ```bash
    ejp-t manage create -h http://your.url.plz -l users_info.csv
    ```

    </br>
1. ```enroll``` : enroll user(s) to workbook. with this command, able to assign workbook to students
    </br>
    * ```-h``` : url of server (cached)
    * ```-u``` : name of a user to enroll
    * ```-r``` : name of workbook to enroll to (cached)
    * ```-l``` : path to .csv file with usernames and workbooks (makes -u and -r optional)
    </br>

    ```bash
    ejp-t manage enroll -h http://your.url.plz -u john1234 -r swe2001_41
    ```
    or
    ```bash
    ejp-t manage enroll -l users_repo.csv
    ```

    </br>
2. ```score``` : get score of user(s)
    </br>
    * ```-h``` : url of server (cached)
    * ```-u``` : username to inspect (overrides -l)
    * ```-r``` : name of workbook to inspect (cached)
    * ```-p``` : name of problem to instpect. without this option, program will generate all scores about problems inside the workbook.
    * ```-l``` : path to store ```[workbook]-[problem].csv```file(e.g. ) which includes score of users. 
    </br>

    ```bash
    ejp-t manage score -u john1234 -r swe2001_41 -p binary_tree
    ```
    or
    ```bash
    ejp-t manage score -p kmp_problem -l ./scores
    ```
    
    </br>
3. ```list``` : get score of user(s)
    </br>
    * ```-h``` : url of server (cached)
    * ```-r``` : name of workbook to inspect
    </br>

    ```bash
    ejp-t manage list
    ```
    or
    ```bash
    ejp-t manage score -r genesisworkbook
    ```
    
    </br>

### workbook
you can manipulate workbook(s).

``` bash
ejp-t workbook [command] [options] 
```
</br>

#### ```[command]```
0. ```create``` : create a new workbook in certain host
    </br>
    * ```-h``` : url of server (cached)
    * ```-l``` : path of workbook directory which possess compenents like `info.json` and problems
    * ```-r``` : name of workbook
    </br>

    ```bash
    ejp-t workbook create -h http:/your.url.plz -l ./workbook0 -r swe2001_43
    ```

    </br>
1. ```delete``` : delete workbook/problem/testcase 
    </br>
    * `-h`: url of server (cached)
    * `-r`: name of workbook. specifies the workbook to delete or the workbook that contains the problem or test cases you want to delete.
    * `-p`: name of problem. specifies the problem to delete or the problem that contains the test cases you want to delete.
    * `-t`: testcase flag. use this flag to indicate that you want to delete test cases. no additional value is needed for this option; its presence is enough to signal the deletion of test cases.

    </br>

    ```bash
    ejp-t workbook delete -r swe2001_42 # delete a whole workbook
    ```
    or
    ```bash
    ejp-t workbook delete -r swe2001_42 -p assignment_1 # delete a problem
    ```
    or
    ```bash
    ejp-t workbook delete -r swe2001_42 -p assignment_1 -t # delete testcases
    ```

    </br>
2. ```append``` : append workbook/problem/testcase 
    </br>
    * ```-h``` : url of server (cached)
    * ```-r``` : name of workbook. specifies the workbook to append problem or testcases. (cached)
    * ```-p``` : name of problem. specifies the problem to append testcases.
    * ```-l``` : path of problem directory or testcases file.
    </br>

    ```bash
    ejp-t workbook append -r swe2001_42 -l ./path_of_problem #append problem to workbook
    ```
    or
    ```bash
    ejp-t workbook append -p assignment_1 -l ./path_of_testcase.json # append testcase to problem
    ```
    
    </br>
