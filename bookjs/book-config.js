paginationConfig = {
    'sectionStartMarker': '.objavi-subsection',
    'sectionTitleMarker': '.objavi-subsection-heading',
    'chapterStartMarker': 'h1',
    'chapterTitleMarker': 'h1',
    'flowElement': 'document.body',
    'alwaysEven': true,
    'divideContents': true,
    'columns': 1,
    'enableFrontmatter': true,
    'bulkPagesToAdd': 50, 
    'pagesToAddIncrementRatio': 1.4, 
    'autoStart': true,
}

document.addEventListener('layoutFlowFinished', function() {
    window.print();
});
