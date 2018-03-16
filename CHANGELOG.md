# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [6.7.0] - 2018-01-24

### Changed
- CMake: Compile and link as Position Independent Code ('-fPIC').
- Add more verbose logging in some areas that required it.
- Debian: Align all version numbers of KMS-related modules.
- Debian: Remove version numbers from package names.
- Debian: Configure builds to use parallel compilation jobs.

### Fixed
- Reset stats after RTP source gets reconnected. The RTP sources assume a starting point of 0, so KMS must also adjust its own status after a reconnection.
- Fix [#197](https://github.com/Kurento/bugtracker/issues/197) (Composite Hub making audio choppy) with [#9](https://github.com/Kurento/kms-core/pull/9) (fix composite: kmsenctreebin.c use max-size-time instead of max-size-buffers), by @ruddell (Jon Ruddell).

## [6.6.3] - 2017-08-10

### Changed
- Prevent frames from building up in the buffer if the CPU falls behind, by @kc7bfi (David Robison).

## [6.6.2] - 2017-07-24

### Added
- REMB: Add "COMEDIA"/automatic port discovery. [TODO Link to documentation].
- REMB: Enable for RTP connections. Previously, it would only work for WebRTC.

### Changed
- Old ChangeLog.md moved to the new format in this CHANGELOG.md file.
- CMake: Full review of all CMakeLists.txt files to tidy up and homogenize code style and compiler flags.
- CMake: Position Independent Code flags ("-fPIC") were scattered around projects, and are now removed. Instead, the more CMake-idiomatic variable "CMAKE_POSITION_INDEPENDENT_CODE" is used.
- CMake: All projects now compile with "[-std=c11|-std=c++11] -Wall -Werror -pthread".
- CMake: Debug builds now compile with "-g -O0" (while default CMake used "-O1" for Debug builds).
- CMake: include() and import() commands were moved to the code areas where they are actually required.
- REMB: Limit estimations to the "double of the max bitrate" only in increment phase.
- REMB: Make "minVideoRecvBandwidth" always >= 30 kbps.

### Fixed
- Bugfix: Out of bound access on SDP medias.
- Use format macros to fix compiler errors on 32bit systems, by @fancycode (Joachim Bauch).
- When a non incremental PTS is discovered in an input stream, the internal DTS gets updated with the new PTS value.

## [6.6.1] - 2016-09-30

### Changed
- Improved thread management when using filters. Filter processing uses its own thread and drops packages that are late.
- Improved compilation issues.

### Fixed
- Fix problem in PTS synchronization algorithm when remote is sending wrong RTCP SR packages that produces backwards PTS.

## [6.6.0] - 2016-09-09

### Added
- Support for UriEndpoint.
- SDP Agent: Add support for error notification using GError, this allows raising betters exceptions to client.
- ServerManager: Add method to get memory used by the server.

### Changed
- SDP Agent: Make code cleaner.
- Improved RTP synchronization algorithm, this makes recorder behave better when recording from RtpEndpoint or WebRtcEndpoint.
- Allow C++ to listen to signals with a return value.
- Updated documentation.
- UriEndpoint: Add property to get state.
- UriEndpoint: Add event to notify state changes.

### Fixed
- Memory problems during Media Elements disconnections.
- Memory problems in flowOut/flowIn events detection.
- Memory leaks.
- Rare media deadlocks on Agnosticbin.
- Improved Media Elements connection, some cases were not working correctly, specially when creating multi stream elements.

## [6.5.0] - 2016-05-27

### Added
- Agnosticbin: Add support for RTP format (only at output).

### Changed
- Changed license to Apache 2.0.
- Updated documentation.
- REMB algorithm improvements.
- Max/min video bandwidth parameters (now 0 means unlimited).
- Raise events from differents threads.

### Fixed
- Bugs in Flow IN - Flow OUT event (caused a segmentation fault).

### Deprecated
- Changed some event/methods names and deprecated old ones (which will be removed on the next major release).

## [6.4.0] - 2016-02-24

### Added
- Prepare implementation to support multistream.
- Add flow in and flow out signals that indicates if there is media going in or out to/from a media element.
- Add leaky queue in filters to avoid them to buffer media if the proccess slower than buffers arrive.

### Changed
- Improve latency stats to add support for multiple streams.
- REMB algorithm improvements.

### Fixed
- Bad timestamp for Opus codec.
- Latency stats calculation.
- Some problems in SDP Agent.

## [6.3.1] - 2016-01-29

### Fixed
- Fix problem with codec names written in lower/upper case.

## [6.3.0] - 2019-01-19

### Added
- SdpEndpoint: Add support for negotiating IPv6 or IPv4.
- Add compilation time to module information (makes debugging easier).
- KurentoException: Add exceptions for player.

### Changed
- Update Glib to 2.46.

### Fixed
- SdpEndpoint: Fix bug on missordered medias when they are bundle.
- Agnosticbin: Fix many negotiation problems caused by new empty caps.
- MediaElement/MediaPipeline: Fix segmentation fault when error event is sent.
- Agnosticbin: Do not negotiate pad until a reconfigure event is received (trying to do so can cause deadlock).
- SDP Agent: Support "mid" without a group (fixes problems with Firefox).
- Fix problem with REMB notifications when we are sending too much NACK events.

## 6.2.0 - 2015-11-25

### Added
- RtpEndpoint: Add address in generated SDP ("0.0.0.0" was added so no media could return to the server).
- SdpEndpoint: Add "maxAudioRecvBandwidth" property.
- BaseRtpEndpoint: Add configuration for port ranges.
- UriEndpoint: Add default uri for relative paths. Now uris without schema are treated with a default value set in configuration. Directory "/var/kurento" is used by default.

### Changed
- Update GStreamer version to 1.7.
- RecorderEndpoint: Change audio format for WEBM from Vorbis to Opus. This is avoiding transcodification and also improving quality.

### Fixed
- Fixed #12 (https://github.com/Kurento/bugtracker/issues/12)
- SdpEndpoint: Raises error when sdp answer cannot be processed.
- MediaPipeline: Add proper error code to error events.
- MediaElement: Fix error notification mechanisms. Errors where not raising in most cases.
- Improvements in format negotiations between elements, this fixes problems in RecorderEndpoint and Composite.

[6.7.0]: https://github.com/Kurento/kms-core/compare/6.6.3...6.7.0
[6.6.3]: https://github.com/Kurento/kms-core/compare/6.6.2...6.6.3
[6.6.2]: https://github.com/Kurento/kms-core/compare/6.6.1...6.6.2
[6.6.1]: https://github.com/Kurento/kms-core/compare/6.6.0...6.6.1
[6.6.0]: https://github.com/Kurento/kms-core/compare/6.5.0...6.6.0
[6.5.0]: https://github.com/Kurento/kms-core/compare/6.4.0...6.5.0
[6.4.0]: https://github.com/Kurento/kms-core/compare/6.3.1...6.4.0
[6.3.1]: https://github.com/Kurento/kms-core/compare/6.3.0...6.3.1
[6.3.0]: https://github.com/Kurento/kms-core/compare/6.2.0...6.3.0
