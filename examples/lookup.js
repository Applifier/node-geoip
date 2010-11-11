var sys = require('sys'),
    ceoip = require('../lib/ceoip');

con = new ceoip.Connection();
con.addListener('connected', function () {
  con.addListener('result', function(result) {
    sys.puts(result);

  });
  var ip = '216.236.135.152';
  sys.puts('Looking up ip: ' + ip + '...\n');
  con.queryCountryCodeByIP(ip);
  con.queryCountryCodeByIP("212.226.93.81");
});
con.connect('/usr/local/share/GeoIP/GeoIP.dat');
