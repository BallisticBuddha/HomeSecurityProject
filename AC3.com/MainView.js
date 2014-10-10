$(function() {
    $( "#date1" ).datepicker();
  });
  
 $(function() {
    $( "#date2" ).datepicker();
  });
  
 $( "#sensor" ).selectmenu();
 
 $( "#deactivated" ).selectmenu();
 
 $(document).ready(function(){
var data = [
    ['User1', 75],['User2', 15],['User3', 30], 
    ['User4', 43],['User5', 42]
  ];
  var plot1 = jQuery.jqplot ('eventactors', [data], 
    { 
	  title: 'User Event Occurrence',
      seriesDefaults: {
        // Make this a pie chart.
        renderer: jQuery.jqplot.PieRenderer, 
        rendererOptions: {
          // Put data labels on the pie slices.
          // By default, labels show the percentage of the slice.
          showDataLabels: true
        }
      },
      legend: { show:true, location: 'e' }
    }
  );
});
