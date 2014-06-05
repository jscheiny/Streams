Titles =
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

makeSidebarSection = ($sidebar, cls)->
    addSidebarTitle $sidebar, cls
    $list = $('<ul/>')
    $list.addClass "sidebar-list"
    $(".function.#{cls}").each (index, el) ->
        addToSidebar $list, $(el)
    $sidebar.append $list

matches = (query, text) -> 
    if query is ''
        return true
    text.indexOf(query) != -1

doSearch = (query) ->
    query = query.toLowerCase()
    $(".sidebar-item .sidebar-anchor").each (index, el) ->
        $el = $(el)
        text = $el.text().toLowerCase()
        if matches query, text
            $el.show()
        else
            $el.hide()

makeKeyPressHandler = ($search) ->
    return (e) ->
        doSearch $search.val()

$ ->
    $sidebar = $('#sidebar')
    makeSidebarSection $sidebar, 'method'
    makeSidebarSection $sidebar, 'generator'
    makeSidebarSection $sidebar, 'operator.stateless'
    makeSidebarSection $sidebar, 'operator.stateful'
    makeSidebarSection $sidebar, 'terminator'
    makeSidebarSection $sidebar, 'exception'
    $search = $('#search')
    $search.keyup makeKeyPressHandler($search)