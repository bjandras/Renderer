
About
=====

This is the Objavi Renderer, which converts HTML files to PDF.
It is a part of the Objavi publishing engine (github.com/sourcefabric/Objavi).

To create the PDF output from HTML pages Objavi Render uses the BookJS
library (github.com/sourcefabric/BookJS) and the WebKit layout engine
(www.webkit.org).

The Objavi Renderer is developed and maintained by Borko Jandras
(borko.jandras@sourcefabric.org).


Copying
=======

Objavi Renderer is free software, distributed under the version 3 of the
GNU Affero General Public License.  The text of the GNU Affero General Public
License is available in the LICENSE.txt file inside the source tree and also
online at http://www.gnu.org/licenses/agpl-3.0.html


Compiling
=========

To compile the Renderer the Qt5 SDK is needed.  For Ubuntu users this
translates to having Qt5 development packages installed from the repository.
Qt5 for Ubuntu is provided by the "Canonical Qt5 Edgers" team at:

    https://launchpad.net/~canonical-qt5-edgers/+archive/qt5-proper

Install the Qt5 packages by adding ppa:canonical-qt5-edgers/qt5-proper to the
list of sources, then install the necessary packages, like this:

    sudo add-apt-repository ppa:canonical-qt5-edgers/qt5-proper
    sudo apt-get install qtbase5-dev libqt5webkit5-dev
    sudo apt-get install qtdeclarative5-dev qtlocation5-dev qtsensors5-dev

Some Qt5 developmet packages are missing some dependancies, so if you get link
errors make sure you have the following packages installed as well:

    sudo apt-get install libxml2-dev libxslt1-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev

To compile the Renderer binary go to the source directory and issue the
following commands:

    QT_SELECT=qt5 qmake
    make


Running
=======

The Renderer can be run in two modes: normal and the GUI mode.

The GUI mode displays a window that looks and behaves just like a normal web
browser would.  This mode is intended for BookJS developers to test how BookJS
behaves inside the WebKit engine.  To do this you can select the
"Install BookJS" button inside the Tools menu.
You can also fire up the standard WebKit developer console and experiment.
To start the Renderer in the GUI mode use the "-gui" command line option.

The normal mode, also called the offline mode, is intended to convert web pages
to PDFs from the command line.  To run the Renderer in this mode specify the
URL of the web page you want to convert to PDF as the command line argument.
Other command line options are:

  * -bookjs -- specifies the location of the BookJS code to use for formatting the page
  * -custom-css -- specifies the CSS text to append after all other CSS text
  * -output -- specified the output file where the PDF should be written to
  * -page-config -- BookJS page configuration (see below)
  * -print-timeout -- number of seconds to wait for the page to be ready for printing

The BookJS page configuration is specified in the following form:

    option1=value1,option2=value2,...

where options are those of the BookJS configuration (see book.js documentation on OPTIONS),
and values are JavaScript floating-point or string literals.
