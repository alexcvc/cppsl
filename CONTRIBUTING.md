# Contribution Guidelines

## Use a Consistent Coding Style

* Use Google++ style with 3 spaces for indentation for C++ files.
* With few exceptions, place code in a namespace. 
* Namespaces should have unique names based on the project name, and possibly its path. 
* Do not use using-directives (e.g., using namespace foo). Do not use inline namespaces.
* Prefer placing nonmember functions in a namespace; use completely global functions rarely. Do not use a class simply to group static members.
* Place a function's variables in the narrowest scope possible, and initialize variables in the declaration.
* Avoid virtual method calls in constructors, and avoid initialization that can fail if you can't signal an error.

## CMake Antipatterns

* Do not use global functions: This includes link_directories, include_libraries, and similar.
* Don't add unneeded PUBLIC requirements: You should avoid forcing something on users that is not required (-Wall). Make these PRIVATE instead.
* Don't GLOB files: Make or another tool will not know if you add files without rerunning CMake. Note that CMake 3.12 adds a CONFIGURE_DEPENDS flag that makes this far better if you need to use it.
* Link to built files directly: Always link to targets if available.
* Never skip PUBLIC/PRIVATE when linking: This causes all future linking to be keyword-less.

## CMake Patterns

* Treat CMake as code: It is code. It should be as clean and readable as all other code.
* Think in targets: Your targets should represent concepts. Make an (IMPORTED) INTERFACE target for anything that should stay together and link to that.
* Export your interface: You should be able to run from build or install.
* Write a Config.cmake file: This is what a library author should do to support clients.
* Make ALIAS targets to keep usage consistent: Using add_subdirectory and find_package should provide the same targets and namespaces.
* Combine common functionality into clearly documented functions or macros: Functions are better usually.
* Use lowercase function names: CMake functions and macros can be called lower or upper case. Always use lower case. Upper case is for variables.
* Use cmake_policy and/or range of versions: Policies change for a reason. Only piecemeal set OLD policies if you have to.

## Naming conventions

Use *short lowercase names* at least for the *top-level files* and folders except `LICENSE`, `README.md`

## Reporting a bug

* Discussing the current state of the code.
* Submitting a fix.
* Proposing new features.
* Becoming a maintainer.
* We use GitHub to host code, to track issues and feature requests, as well as accept pull requests.

## Write bug reports with detail, background, and sample code

Great Bug Reports tend to have:

* A quick summary and/or background.
* Steps to reproduce.
* Be specific!
* Give sample code if you can. 
* What you expected would happen.
* What actually happens.
* Notes (possibly including why you think this might be happening, or stuff you tried that didn't work).

## All Code Changes Happen Through Pull Requests

Pull requests are the best way to propose changes to the codebase.

* Fork the repo and create your branch from master.
* If you've added code that should be tested, add tests.
* If you've changed APIs, update the documentation.
* Ensure the test suite passes.
* Make sure your code lints.
* Issue that pull request!

