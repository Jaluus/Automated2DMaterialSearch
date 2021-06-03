$("#response").html("lel");

$("#lolbut").click(() => {
    $("#response").html("LOL")
}
)

// sorting 
$('th').click(function () {
    var table = $(this).parents('table').eq(0)
    var rows = table.find('tr:gt(0)').toArray().sort(comparer($(this).index()))
    this.asc = !this.asc
    if (!this.asc) { rows = rows.reverse() }
    for (var i = 0; i < rows.length; i++) { table.append(rows[i]) }
})
function comparer(index) {
    return function (a, b) {
        var valA = getCellValue(a, index), valB = getCellValue(b, index)
        return $.isNumeric(valA) && $.isNumeric(valB) ? valA - valB : valA.toString().localeCompare(valB)
    }
}
function getCellValue(row, index) { return $(row).children('td').eq(index).text() }



function responseHandler(response) {

    //response = $.parseJSON(response);
    //console.log(response)

    if (response.length === 0) {
        $('<tr id="table_values">').append(
            $('<td>').text("No flakes"),
            $('<td>').text("-"),
            $('<td>').text("-")
        ).appendTo('#response_table');
    }


    $.each(response, function (index, responseItem) {
        $('<tr id="table_values">').append(
            $('<td>').text(responseItem.path),
            $('<td>').text(responseItem.thickness),
            $('<td>').text(responseItem.size)
        ).appendTo('#response_table');
        //console.log($tr.wrap('<p>').html());
    });
}

//$('<td>').html($('<img>').attr('src', "lol.png")),

function clearList() {
    $("#response_table #table_values").remove()
}



$("#getbut").click(() => {
    var id = $("#img_id").val()
    $.get(`http://127.0.0.1:3002/image/?id=${id}`, (data) => {
        clearList()

        responseHandler(data)
    }
    )
}
)
