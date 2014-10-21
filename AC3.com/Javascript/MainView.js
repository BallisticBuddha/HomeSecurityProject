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
    ['Alex', 75],['Chris1', 65],['Chris2', 30], 
    ['Chance', 43]
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
  
  var data2 = [
    ['Kitchen', 10],['Dinning', 36],['Bedroom', 30], 
    ['Garage', 25],['Deck', 50]
  ];
  var plot2 = jQuery.jqplot ('sensors', [data2], 
    { 
	  title: 'Sensor Event Occurrence',
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
