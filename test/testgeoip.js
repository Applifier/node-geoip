var geoip = require('../geoip');
var settings = require('../../settings.js');



exports.testGeoIp = function (test) {
	var geoipConnection = new geoip.GeoIP();

	geoipConnection.open(settings.get('geoip_path'), function(err) {
		var country = geoipConnection.query("212.226.93.89");
		test.equals(country, "FI", "212.226.93.89 should be in FI");
		test.done();
	});



};


exports.testGeoIpUnknown = function (test) {
	var geoipConnection = new geoip.GeoIP();

	geoipConnection.open(settings.get('geoip_path'), function(err) {
		var country = geoipConnection.query("127.0.0.1");
		test.equals(country, null, "127.0.0.1 should return null");
		test.done();
	});



};

exports.testGeoIpContinent = function (test) {
	var geoipConnection = new geoip.GeoIP();

	geoipConnection.open(settings.get('geoip_path'), function(err) {
		var res = geoipConnection.advQuery("212.226.93.89");
		test.equals(res.country_code, "FI", "212.226.93.89 country should be FI");
		test.equals(res.continent, "EU", "212.226.93.89 region should be EU");

		var res = geoipConnection.advQuery("207.46.197.32"); // microsoft.com should be NA
		test.equals(res.continent, "NA", "207.46.197.32 region should be NA");

		var res = geoipConnection.advQuery("127.0.0.1");
		test.equals(res, null, "127.0.0.1 result should be NULL");


		test.done();
	});


};

