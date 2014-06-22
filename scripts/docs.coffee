Titles =
    'top':
        title: 'C++ Streams'
        link:  ''
    'intro':
        title: 'Introduction'
        link:  'intro'
    'method':
        title: 'Methods'
        link:  'methods'
    'generator':
        title: 'Generators'
        link:  'generators'
    'operator.stateless':
        title: 'Stateless Intermediate Operators'
        link:  'stateless'
    'operator.stateful':
        title: 'Stateful Intermediate Operators'
        link:  'stateful'
    'terminator':
        title: 'Terminal Operators'
        link:  'terminals'
    'exception':
        title: 'Exceptions'
        link:  'exceptions'
    'recipes':
        title: 'Recipes'
        link:  'recipes'

addSidebarTitle = ($sidebar, cls) ->
    title = $ "<a/>"
    title.addClass 'sidebar-title'
    title.attr 'href', '#' + Titles[cls].link
    title.html Titles[cls].title
    $sidebar.append title

addToSidebar = ($list, $func) ->
    id = $func.attr 'id'
    $item = $ '<li/>'
    $item.addClass 'sidebar-item'
    $anchor = $ '<a/>'
    $anchor.addClass 'sidebar-anchor'
    $anchor.html $('.fname', $func).html()
    $anchor.attr 'href', '#' + id
    $item.append $anchor
    $list.append $item

makeLinkSidebarSection = ($sidebar, cls) ->
    addSidebarTitle $sidebar, cls

makeFunctionSideBarSection = ($sidebar, cls)->
    addSidebarTitle $sidebar, cls
    $list = $('<ul/>')
    $list.addClass "sidebar-list"
    $(".function.#{cls}").each (index, el) ->
        addToSidebar $list, $(el)
    $sidebar.append $list

$ ->
    $sidebar = $('#sidebar')
    makeLinkSidebarSection $sidebar, 'top'
    makeLinkSidebarSection $sidebar, 'intro'
    makeFunctionSideBarSection $sidebar, 'method'
    makeFunctionSideBarSection $sidebar, 'generator'
    makeFunctionSideBarSection $sidebar, 'operator.stateless'
    makeFunctionSideBarSection $sidebar, 'operator.stateful'
    makeFunctionSideBarSection $sidebar, 'terminator'
    makeFunctionSideBarSection $sidebar, 'exception'
    makeLinkSidebarSection $sidebar, 'recipes'
