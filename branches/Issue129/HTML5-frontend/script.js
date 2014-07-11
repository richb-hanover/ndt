
$(function(){
  jQuery.fx.interval = 50;
  if (simulate) {
    setTimeout(initializeTest, 1000);
    return;
  }
  initializeTest();
});



// CONSTANTS

// Testing phases

var PHASE_LOADING   = 0;
var PHASE_WELCOME   = 1;
var PHASE_PREPARING = 2;
var PHASE_UPLOAD    = 3;
var PHASE_DOWNLOAD  = 4;
var PHASE_RESULTS   = 5;


// STATUS VARIABLES

var currentPhase = PHASE_LOADING;
var currentPage = 'welcome';
var transitionSpeed = 400;

// Gauges used for showing download/upload speed
var downloadGauge, uploadGauge;
var gaugeUpdateInterval;
var gaugeMaxValue = 100; // Mb/s


// PRIMARY METHODS

function initializeTest() {

  // Initialize gauges
  initializeGauges();

  // Initialize start buttons
  $('.start.button').click(startTest);

  // Results view selector
  $("#results .view-selector .summary").click(showResultsSummary);
  $("#results .view-selector .details").click(showResultsDetails);
  $("#results .view-selector .advanced").click(showResultsAdvanced);

  document.body.className = 'ready';
  //return showPage('results');
  setPhase(PHASE_WELCOME);
}

function startTest(evt) {
  evt.stopPropagation();
  evt.preventDefault();
  showPage('test', resetGauges);
  document.getElementById('rttValue').innerHTML = "";
  if (simulate) return simulateTest();
  currentPhase = PHASE_WELCOME;
  testNDT().run_test();
  monitorTest();
}

function simulateTest() {
  setPhase(PHASE_RESULTS);
  return;
  setPhase(PHASE_PREPARING);
  setTimeout(function(){ setPhase(PHASE_UPLOAD) }, 2000);
  setTimeout(function(){ setPhase(PHASE_DOWNLOAD) }, 4000);
  setTimeout(function(){ setPhase(PHASE_RESULTS) }, 6000);
}

function monitorTest() {
  var message = testError();
  var currentStatus = testStatus();

  /*
  var currentStatus = testStatus();
  debug(currentStatus);
  var diagnosis = testDiagnosis();
  debug(diagnosis);
  */

  if (message.match(/not run/) && currentPhase != PHASE_LOADING) {
    setPhase(PHASE_WELCOME);
    return false;
  }
  if (message.match(/completed/) && currentPhase < PHASE_RESULTS) {
    setPhase(PHASE_RESULTS);
    return true;
  }
  if (currentStatus.match(/Outbound/) && currentPhase < PHASE_UPLOAD) {
    setPhase(PHASE_UPLOAD);
  }
  if (currentStatus.match(/Inbound/) && currentPhase < PHASE_DOWNLOAD) {
    setPhase(PHASE_DOWNLOAD);
  }

  if (!currentStatus.match(/Middleboxes/) && !currentStatus.match(/notStarted/) 
        && !remoteServer().match(/ndt/) && currentPhase == PHASE_PREPARING) {
    debug("Remote server is " + remoteServer());
    setPhase(PHASE_UPLOAD);
  }
  if (remoteServer() !== 'unknown' && currentPhase < PHASE_PREPARING) {
    setPhase(PHASE_PREPARING);
  }
  setTimeout(monitorTest, 100);
}



// PHASES

function setPhase(phase) {
  switch (phase) {

    case PHASE_WELCOME:
      debug("WELCOME");
      showPage('welcome');
      break;

    case PHASE_PREPARING:
      uploadGauge.setValue(0);
      downloadGauge.setValue(0);
      debug("PREPARING TEST");

      $('#loading').show();
      $('#upload').hide();
      $('#download').hide();

      showPage('test', resetGauges);
      break;

    case PHASE_UPLOAD:
      var pcBuffSpdLimit, rtt, gaugeConfig = [];
      debug("UPLOAD TEST");

      pcBuffSpdLimit = speedLimit();
      rtt = averageRoundTrip();

      if (isNaN(rtt)) {
        document.getElementById("rttValue").innerHTML = "n/a";
      } else {
        document.getElementById("rttValue").innerHTML = rtt.toFixed(2).concat(" ms");
      }

      if (!isNaN(pcBuffSpdLimit)) {
        if (pcBuffSpdLimit > gaugeMaxValue) {
          pcBuffSpdLimit = gaugeMaxValue; 
        }
        gaugeConfig.push({
          from: 0,   to: pcBuffSpdLimit, color: 'rgb(0, 255, 0)'
        });

        gaugeConfig.push({
          from: pcBuffSpdLimit, to: gaugeMaxValue, color: 'rgb(255, 0, 0)'
        });

        uploadGauge.updateConfig({ 
          highlights: gaugeConfig
        });

        downloadGauge.updateConfig({ 
          highlights: gaugeConfig
        });
      }

      $('#loading').hide();
      $('#upload').show();

      gaugeUpdateInterval = setInterval(function(){
        updateGaugeValue();
      },1000);

      $("#test .remote.location .address").get(0).innerHTML = remoteServer();
      break;

    case PHASE_DOWNLOAD:
      debug("DOWNLOAD TEST");

      $('#upload').hide();
      $('#download').show();
      break;

    case PHASE_RESULTS:
      debug("SHOW RESULTS");
      debug('Testing complete');

      document.getElementById('upload-speed').innerHTML = uploadSpeed().toPrecision(2); 
      document.getElementById('download-speed').innerHTML = downloadSpeed().toPrecision(2); 
      document.getElementById('latency').innerHTML = averageRoundTrip().toPrecision(2); 
      document.getElementById('jitter').innerHTML = jitter().toPrecision(2); 
      document.getElementById("test-details").innerHTML = testDetails();
      document.getElementById("test-advanced").innerHTML = testDiagnosis();

      showPage('results');
      break;

    default:
      return false;
  }
  currentPhase = phase;
}


// PAGES

function showPage(page, callback) {
  debug("Show page: " + page);
  if (page == currentPage) {
    if (callback !== undefined) callback();
    return true;
  }
  if (currentPage !== undefined) {
    $("#" + currentPage).fadeOut(transitionSpeed, function(){
      $("#" + page).fadeIn(transitionSpeed, callback);
    });
  }
  else {
    debug("No current page");
    $("#" + page).fadeIn(transitionSpeed, callback);
  }
  currentPage = page;
}


// RESULTS

function showResultsSummary() {
  showResultsPage("summary");
}

function showResultsDetails() {
  showResultsPage("details");
}

function showResultsAdvanced() {
  showResultsPage("advanced");
}

function showResultsPage(page) {
  debug("Results: show " + page);
  var pages = ["summary", "details", "advanced"];
  for (var i=0, len=pages.length; i < len; i++) {
    $("#results")[(page == pages[i]) ? "addClass" : "removeClass"](pages[i]);
  }
}


// GAUGE

function initializeGauges() {
  var gaugeValues = [];

  for (var i=0; i<=10; i++) {
    gaugeValues.push(0.1 * gaugeMaxValue * i);
  }
  uploadGauge = new Gauge({
    renderTo    : 'uploadGauge',
    width       : 270,
    height      : 270,
    units       : 'Mb/s',
    title       : "Upload",
    minValue    : 0,
    maxValue    : gaugeMaxValue,
    majorTicks  : gaugeValues,
    highlights  : [{ from: 0, to: gaugeMaxValue, color: 'rgb(0, 255, 0)' }]
  });;

  gaugeValues = [];
  for (var i=0; i<=10; i++) {
    gaugeValues.push(0.1 * gaugeMaxValue * i);
  }
  downloadGauge = new Gauge({
    renderTo    : 'downloadGauge',
    width       : 270,
    height      : 270,
    units       : 'Mb/s',
    title       : "Download",
    minValue    : 0,
    maxValue    : gaugeMaxValue,
    majorTicks  : gaugeValues,
    highlights  : [{ from: 0, to: gaugeMaxValue, color: 'rgb(0, 255, 0)' }]
  });;
}

function resetGauges() {
  var gaugeConfig = [];

  gaugeConfig.push({
    from: 0, to: gaugeMaxValue, color: 'rgb(0, 255, 0)'
  });  

  uploadGauge.updateConfig({
    highlights: gaugeConfig
  });
  uploadGauge.setValue(0);

  downloadGauge.updateConfig({ 
    highlights: gaugeConfig
  });
  downloadGauge.setValue(0);
}

function updateGaugeValue() {
  if (currentPhase == PHASE_UPLOAD) {
    uploadGauge.setValue(uploadSpeed(true));
  } else if (currentPhase == PHASE_DOWNLOAD) {
    downloadGauge.setValue(downloadSpeed());
  } else {
    clearInterval(gaugeUpdateInterval);  
  }   
}

// TESTING JAVA/FLASH CLIENT

function testNDT() {
  return ndt = document.getElementById('NDT');
}

function testStatus() {
  return testNDT().get_status();
}

function testDiagnosis() {
  if (simulate) return "Test diagnosis";
  return testNDT().get_diagnosis();
}

function testError() {
  return testNDT().get_errmsg();
}

function remoteServer() {
  if (simulate) return '0.0.0.0';
  return testNDT().get_host();
}

function uploadSpeed(raw) {
  if (simulate) return 0;
  var speed = testNDT().getNDTvar("ClientToServerSpeed");
  return raw ? speed : parseFloat(speed);
}

function downloadSpeed() {
  if (simulate) return 0;
  return parseFloat(testNDT().getNDTvar("ServerToClientSpeed"));
}

function averageRoundTrip() {
  if (simulate) return 0;
  return parseFloat(testNDT().getNDTvar("avgrtt"));
}

function jitter() {
  if (simulate) return 0;
  return parseFloat(testNDT().getNDTvar("Jitter"));
}

function speedLimit() {
  if (simulate) return 0;
  return parseFloat(testNDT().get_PcBuffSpdLimit());
}

function testDetails() {
  if (simulate) return 'Test details';

  var a = testNDT();
  var d = '';

  d += "Your system: " + a.getNDTvar("OperatingSystem") + "<br>";
  d += "Plugin version: " + a.getNDTvar("PluginVersion") + " (" + a.getNDTvar("OsArchitecture") + ")<br>";

  d += "<br>";

  d += "TCP receive window: " + a.getNDTvar("CurRwinRcvd") + " current, " + a.getNDTvar("MaxRwinRcvd") + " maximum<br>";
  d += a.getNDTvar("loss") + " packets lost during test<br>";
  d += "Round trip time: " + a.getNDTvar("MinRTT") + " msec (minimum), " + a.getNDTvar("MaxRTT") + " msec (maximum), " + a.getNDTvar("avgrtt") + " msec (average)<br>";
  d += "Jitter: " + a.getNDTvar("Jitter") + " msec<br>";
  d += a.getNDTvar("waitsec") + " seconds spend waiting following a timeout<br>";
  d += "TCP time-out counter: " + a.getNDTvar("CurRTO") + "<br>";
  d += a.getNDTvar("SACKsRcvd") + " selective acknowledgement packets received<br>";

  d += "<br>";

  if (a.getNDTvar("mismatch") == "yes") {
    d += "A duplex mismatch condition was detected.<br>";
  }
  else {
    d += "No duplex mismatch condition was detected.<br>";
  }

  if (a.getNDTvar("bad_cable") == "yes") {
    d += "The test detected a cable fault.<br>";
  }
  else {
    d += "The test did not detect a cable fault.<br>";
  }

  if (a.getNDTvar("congestion") == "yes") {
    d += "Network congestion may be limiting the connection.<br>";
  }
  else {
    d += "No network congestion was detected.<br>";
  }

  /*if (a.get_natStatus() == "yes") {
    d += "A network address translation appliance was detected.<br>";
  }
  else {
    d += "No network addess translation appliance was detected.<br>";
  }*/

  d += "<br>";

  d += a.getNDTvar("cwndtime") + "% of the time was not spent in a receiver limited or sender limited state.<br>";
  d += a.getNDTvar("rwintime") + "% of the time the connection is limited by the client machine's receive buffer.<br>";
  d += "Optimal receive buffer: " + a.getNDTvar("optimalRcvrBuffer") + " bytes<br>";
  d += "Bottleneck link: " + a.getNDTvar("accessTech") + "<br>";
  d += a.getNDTvar("DupAcksIn") + " duplicate ACKs set<br>";

  return d;
}


// UTILITIES

function debug(message) {
  if (allowDebug && window.console) console.debug(message);
}
