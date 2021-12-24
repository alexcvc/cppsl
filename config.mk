
.PHONY: submodules
submodules: doctest gsl gsl-lite spdlog pugixml nlohmann_json cxxopts

.PHONY: doctest
doctest:
	git submodule add -b master --name doctest https://github.com/onqtam/doctest.git external/doctest
	git submodule update --init

.PHONY: gsl
gsl:
	git submodule add -b main --name gsl git@github.com:microsoft/GSL.git external/gsl
	git submodule update --init

.PHONY: gsl-lite
gsl-lite:
	git submodule add -b master --name gsl-lite git@github.com:gsl-lite/gsl-lite.git external/gsl-lite
	git submodule update --init

.PHONY: spdlog
spdlog:
	git submodule add   -b master --name spdlog git@github.com:gabime/spdlog.git external/spdlog
	git submodule update --init

.PHONY: pugixml
pugixml:
	git submodule add -b master --name pugixml git@github.com:zeux/pugixml.git external/pugixml
	git submodule update --init

.PHONY: nlohmann_json
nlohmann_json:
	git submodule add -b master --name nlohmann_json git@github.com:nlohmann/json.git external/nlohman_json
	git submodule update --init

.PHONY: cxxopts
cxxopts:
	git submodule add -b master --name cxxopts https://github.com/jarro2783/cxxopts external/cxxopts
	git submodule update --init

.PHONY: fmt
fmt:
	git submodule add -b master --name fmt git@github.com:fmtlib/fmt.git external/fmt
	git submodule update --init

.PHONY: clean
clean:
