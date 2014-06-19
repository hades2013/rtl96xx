
var Customer_Manual_Showing = "显示"; 
var Customer_Manual_ShowingOf = "总共"; 
var Customer_Manual_ShowAll = "所有"; 
var Customer_Manual_ShowPerPage = "页"; 
var Customer_Manual_FirstPage = '第一页'; 
var Customer_Manual_PreviousPage = "返回"; 
var Customer_Manual_Page = "页数"; 
var Customer_Manual_NextPage = "Next"; 
var Customer_Manual_LastPage = "Last";
function mk_span(base, tid, val)
{
  return "<span id='" + base + tid + "'>" + val + "</span>";
}

function PaginationHeaderBuild(tid, itemsPerPage, currentPage, allItems, paginatorValues)
{
  var pageDivHeaderClass = "xuiPaginationHeader" + tid;

  $("." + pageDivHeaderClass).html("");

  var style = "<table cellpadding=0 cellspacing=0><tr>";
  var startIndex = (currentPage-1)*itemsPerPage + 1;
  var endIndex = startIndex + itemsPerPage - 1;
  if(endIndex > allItems)
    endIndex = allItems;

  style += "<td class='pageItemCount'>" + Customer_Manual_Showing + " "
        + mk_span("pageItemStartIndex", tid, startIndex) + "-" + mk_span("pageItemEndIndex", tid, endIndex) + " "
        + Customer_Manual_ShowingOf + " " + mk_span("pageItemAllItems", tid, allItems) + " " + "</td>";
  style += "<td class=pageRowPerPageSelect><"+"select id=pageRowPerPageSelectID" + tid + ">";

  var selected = false;
  for(var i=0;i<paginatorValues.length;i++)
  {
    if(itemsPerPage == paginatorValues[i])
    {
      style += '<'+'option value="'+paginatorValues[i]+'" selected="selected">'+paginatorValues[i]+'<'+'/option>';
      selected = true;
    }
    else if(allItems >= paginatorValues[i])
    {
      style += '<'+'option value="'+paginatorValues[i]+'">'+paginatorValues[i]+'<'+'/option>';
    }
    else
    {
      break;
    }
  }
  
  if(allItems < 100)
  {
  if(selected == false)
  {
    style += "<"+"option value='"+allItems+"' selected='selected'>" + Customer_Manual_ShowAll + "<"+"/option>";
  }
  else
  {
    style += "<"+"option value='"+allItems+"'>" + Customer_Manual_ShowAll + "<"+"/option>";
  }
  }
  style += "<"+"/select></td><td class=pageRowPerPageText>" + Customer_Manual_ShowPerPage + "</td>";

  style = style + "</tr></table>";

  $("." + pageDivHeaderClass).html(style);
}


function mk_button_image(txt, img, cls, id, hide)
{
  var style = hide ? "style='display:none;'" : "";
  return "<td class=pageControlsImage><a href='#' " + style + " class='" + cls + "' id='" + id +
         "' title='" + txt + "'> <IMG SRC='image/" + img + ".gif' border=0 /></a></td>";
}

function PaginationFooterBuild(tid, itemsPerPage, currentPage, allItems, paginatorValues)
{
  var pageDivFooterClass = "xuiPaginationFooter" + tid;
  var lastPage = parseInt((allItems-1)/itemsPerPage)+1;

  $("." + pageDivFooterClass).html("");

  var content = "<table cellpadding=0 cellspacing=0 ><tr>";

  if(currentPage == 1)
  {
    content += mk_button_image(Customer_Manual_FirstPage, "First-D", "inactive", "pageControlsImageDisabledFirstID" + tid, false);
    content += mk_button_image(Customer_Manual_FirstPage, "First", "active", "pageControlsImageFirstID" + tid, true);
    content += mk_button_image(Customer_Manual_PreviousPage, "Previous-D", "inactive", "pageControlsImageDisabledPrevID" + tid, false);
    content += mk_button_image(Customer_Manual_PreviousPage, "Previous", "active", "pageControlsImagePrevID" + tid, true);
  }
  else
  {
    content += mk_button_image(Customer_Manual_FirstPage, "First-D", "inactive", "pageControlsImageDisabledFirstID" + tid, true);
    content += mk_button_image(Customer_Manual_FirstPage, "First", "active", "pageControlsImageFirstID" + tid, false);
    content += mk_button_image(Customer_Manual_PreviousPage, "Previous-D", "inactive", "pageControlsImageDisabledPrevID" + tid, true);
    content += mk_button_image(Customer_Manual_PreviousPage, "Previous", "active", "pageControlsImagePrevID" + tid, false);
  }

  content += "<td class=pageControlsText1>" + Customer_Manual_Page + "</td>";
  content += "<td class=pageControlsSelect><"+"select id=pageControlsSelectID" + tid + ">";
  for(var i=1;i<=lastPage;i++)
  {
    if(i == currentPage)
    {
      content += '<'+'option value="'+i+'" selected="selected">'+i+'<'+'/option>';
    }
    else
    {
      content += '<'+'option value="'+i+'">'+i+'<'+'/option>';
    }
  }
  content += '<'+'/select></td>';
  content += "<td class='pageControlsText2'>" + Customer_Manual_ShowingOf + " " 
          + mk_span("pageItemLastPage", tid, lastPage) + "</td>";

  if(currentPage == lastPage)
  {
    content += mk_button_image(Customer_Manual_NextPage, "Next-D", "inactive", "pageControlsImageDisabledNextID" + tid, false);
    content += mk_button_image(Customer_Manual_NextPage, "Next", "active", "pageControlsImageNextID" + tid, true);
    content += mk_button_image(Customer_Manual_LastPage, "Last-D", "inactive", "pageControlsImageDisabledLastID" + tid, false);
    content += mk_button_image(Customer_Manual_LastPage, "Last", "active", "pageControlsImageLastID" + tid, true);
  }
  else
  {
    content += mk_button_image(Customer_Manual_NextPage, "Next-D", "inactive", "pageControlsImageDisabledNextID" + tid, true);
    content += mk_button_image(Customer_Manual_NextPage, "Next", "active", "pageControlsImageNextID" + tid, false);
    content += mk_button_image(Customer_Manual_LastPage, "Last-D", "inactive", "pageControlsImageDisabledLastID" + tid, true);
    content += mk_button_image(Customer_Manual_LastPage, "Last", "active", "pageControlsImageLastID" + tid, false);
  }

  content += "</tr></table>";

  $("." + pageDivFooterClass).html(content);
}

function PaginationPerPageSelector(tid, func)
{
  $("#pageRowPerPageSelectID" + tid).change(function() {
      var selected = $("#pageRowPerPageSelectID" + tid + " option:selected");
      if(selected.val() != 0){
        func(Number(selected.val()));
      }
  });
}

function PaginationPageSelector(tid, func)
{
  $("#pageControlsSelectID" + tid).change(function() {
    var selected = $("#pageControlsSelectID" + tid + " option:selected");
    if(selected.val() != 0){
      func(Number(selected.val()));
    }
  });
}

function PaginationButtonHandlers(tid, func)
{
  $("#pageControlsImageFirstID" + tid).click(function() { func("first"); });
  $("#pageControlsImageLastID" + tid).click(function() { func("last"); });
  $("#pageControlsImagePrevID" + tid).click(function() { func("previous"); });
  $("#pageControlsImageNextID" + tid).click(function() { func("next"); });
}

function PaginationFieldsUpdate(tid, itemsPerPage, currentPage, allItems, paginatorValues)
{
  var pageDivHeaderClass = "xuiPaginationHeader" + tid;

  var startIndex = (currentPage-1)*itemsPerPage + 1;
  var endIndex = startIndex + itemsPerPage - 1;
  var lastPage = parseInt((allItems-1)/itemsPerPage)+1;

  if(endIndex > allItems)
    endIndex = allItems;

  $("#pageItemStartIndex" + tid).html(startIndex);
  $("#pageItemEndIndex" + tid).html(endIndex);
  $("#pageItemAllItems" + tid).html(allItems);
  $("#pageItemLastPage" + tid).html(lastPage);

  if(currentPage == 1)
  {
    $("#pageControlsImageDisabledFirstID" + tid).show();
    $("#pageControlsImageDisabledPrevID" + tid).show();
    $("#pageControlsImageFirstID" + tid).hide();
    $("#pageControlsImagePrevID" + tid).hide();
  }
  else
  {
    $("#pageControlsImageDisabledFirstID" + tid).hide();
    $("#pageControlsImageDisabledPrevID" + tid).hide();
    $("#pageControlsImageFirstID" + tid).show();
    $("#pageControlsImagePrevID" + tid).show();
  }

  if(currentPage == lastPage)
  {
    $("#pageControlsImageDisabledNextID" + tid).show();
    $("#pageControlsImageDisabledLastID" + tid).show();
    $("#pageControlsImageNextID" + tid).hide();
    $("#pageControlsImageLastID" + tid).hide();
  }
  else
  {
    $("#pageControlsImageDisabledNextID" + tid).hide();
    $("#pageControlsImageDisabledLastID" + tid).hide();
    $("#pageControlsImageNextID" + tid).show();
    $("#pageControlsImageLastID" + tid).show();
  }
 
  var version = 0;
  if (navigator.appVersion.indexOf("MSIE") != -1)
    version = parseFloat(navigator.appVersion.split("MSIE")[1]);

  if(version != 6)
  {
    $("#pageControlsSelectID" + tid).empty();

    for(var i=1;i<=lastPage;i++)
    {
      if(i == currentPage)
      {
        $("#pageControlsSelectID" + tid).append ('<'+'option value="'+i+'" selected="selected">'+i+'<'+'/option>');
      }
      else
      {
        $("#pageControlsSelectID" + tid).append ('<'+'option value="'+i+'">'+i+'<'+'/option>');
      }
    }
  }
  else
  {
    var combo = $("#pageControlsSelectID" + tid);
    var id = currentPage - 1;
    combo[0].options[id].selected = true;
  }
}
