# Daft Mover

**Supports Unreal Engine 5.5**

**! Main is not currently stable, use with extreme caution. For stable version, use the FGMovement branch !**

Custom implementation ontop of Mover.

Worth noting that this plugin was originally made when Mover was still on main, so needless to say since this time there has been some extremely breaking API changes, if you used this plugin previously and are now upgrading to newer versions - expect that almost everything will be broken and you have a lot of work to do, as such the old version of this plugin "FGMovement" for 5.4 has been deprecated under a branch, going forward main will contain "Daft Mover" which you can consider the 2.0 of the original plugin with the severely breaking changes bringing it up to compatibility with the newer API.

This Mover implementation is primarily being used to Pioneer a custom Kinematic Liason closer to the original CMC, instead of the default option for kinematic movement which is the Network Prediction Liason. The primary reason that this repository went stale for a while is because frankly, I wasn't very impressed with the performance of NPP, and the development on it didn't look very promising at all. However I LOVE Mover, it could be something great but feels weighed down by the baggage of a failed experiment. As such this repo will be trying a very destructive method to solve this issue. It will definitely be opinionated. It will probably be spicy. Feel free to use it.
