pagination.sectionStartMarker = '.objavi-subsection';
pagination.sectionTitleMarker = '.objavi-subsection-heading';
pagination.chapterStartMarker = 'h1';
pagination.chapterTitleMarker = 'h1';
pagination.autoStart          = true;
pagination.alwaysEven         = true;
pagination.enableReflow       = false;
pagination.enableFrontmatter  = true;

//pagination.frontmatterContents = '<div id="booktitle">Book title</div><div id="booksubtitle">Book subtitle</div><div id="bookeditors">ed. Editor 1, Editor II, Editor III</div><div class="pagebreak"></div><div id="copyrightpage">Copyright: You<br>License: CC</div><div class="pagebreak"></div>';

document.body.addEventListener('layoutFlowFinished', function() {
    window.print();
});
