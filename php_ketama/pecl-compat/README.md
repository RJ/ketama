The PECL compatibility library is a set of C include files whose objective is to minimize the pain of making a PHP extension compatible with PHP 5 and 7.

Project home : [https://github.com/flaupretre/pecl-compat](https://github.com/flaupretre/pecl-compat)

# Main features

- A set of miscellaneous compatibility macros,

- A backport of the *zend_string* functions and macros to PHP 5,

- A backport of most PHP 7 *zend_hash* features to PHP 5.

- A compatibility library for simple resource handling

# Examples

For examples of using this library, look at the [PCS extension](https://github.com/flaupretre/pecl-pcs). There, you will see usage examples of most *pecl-compat* features.

# Usage

Using the library is simple. Download the latest release from the [github repository](https://github.com/flaupretre/pecl-compat/releases) and insert it a new subdirectory inside your code tree. Then, include the 'compat.h' file in every '.c' source file.

# History

I first tried to port the [PHK](http://pecl.php.net/package/phk) extension to PHP 7 using raw '#ifdef' directives. Unfortunately, it quickly became clear that it was not possible without making the code totally unreadable. For pure 'bridge' extensions, it may be possible. But, as soon as you make use of hash tables and strings, your code quicly becomes very hard to maintain.

Then, I explored other solutions : separate branches, duplicate code trees... without finding one I would be satisfied with, mostly because all these solutions bring their own set of maintainability issues : separate branches bring a huge versioning problem, and separate code trees are a maintenance headache in the long term. As PHP 5 modules will probably need to be maintained during years, none of these solutions looked convincing to me.

So, I reverted to the solution of keeping a single source tree and move most of my conditional code to a reusable compatibility layer. In order to benefit of the PHP 7 performance increase, most pecl-compat features are PHP 7 features backported to PHP 5. PHP 7 generally calls the underlying PHP functions directly, with the same performance as a pure-PHP7 development. So, when adapting your extension to use pecl-compat, you will, at the same time, change your code to use new PHP 7 features and make it compatible with both PHP 5 & 7. As a side effect, this makes it easier to discard PHP 5 compatibility in the future, when time has come.

# Supported PHP versions

5.3 and above.
