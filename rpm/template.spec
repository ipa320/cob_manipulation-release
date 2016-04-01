Name:           ros-indigo-cob-collision-monitor
Version:        0.6.4
Release:        0%{?dist}
Summary:        ROS cob_collision_monitor package

Group:          Development/Libraries
License:        LGPLv3
URL:            http://wiki.ros.org/cob_collision_monitor
Source0:        %{name}-%{version}.tar.gz

Requires:       ros-indigo-cob-moveit-config
Requires:       ros-indigo-moveit-ros-move-group
Requires:       ros-indigo-moveit-ros-planning
Requires:       ros-indigo-pluginlib
Requires:       ros-indigo-std-msgs
Requires:       ros-indigo-tf
BuildRequires:  ros-indigo-catkin
BuildRequires:  ros-indigo-moveit-ros-move-group
BuildRequires:  ros-indigo-moveit-ros-planning
BuildRequires:  ros-indigo-pluginlib
BuildRequires:  ros-indigo-std-msgs
BuildRequires:  ros-indigo-tf

%description
The collision monitor uses the planning scene monitor to read the state of the
robot and check it for collision with itselt or the environment. It addition a
ground plane is added in any case. Can be used as a stand-aloan node or a
move_group capability.

%prep
%setup -q

%build
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/indigo/setup.sh" ]; then . "/opt/ros/indigo/setup.sh"; fi
mkdir -p obj-%{_target_platform} && cd obj-%{_target_platform}
%cmake .. \
        -UINCLUDE_INSTALL_DIR \
        -ULIB_INSTALL_DIR \
        -USYSCONF_INSTALL_DIR \
        -USHARE_INSTALL_PREFIX \
        -ULIB_SUFFIX \
        -DCMAKE_INSTALL_LIBDIR="lib" \
        -DCMAKE_INSTALL_PREFIX="/opt/ros/indigo" \
        -DCMAKE_PREFIX_PATH="/opt/ros/indigo" \
        -DSETUPTOOLS_DEB_LAYOUT=OFF \
        -DCATKIN_BUILD_BINARY_PACKAGE="1" \

make %{?_smp_mflags}

%install
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/indigo/setup.sh" ]; then . "/opt/ros/indigo/setup.sh"; fi
cd obj-%{_target_platform}
make %{?_smp_mflags} install DESTDIR=%{buildroot}

%files
/opt/ros/indigo

%changelog
* Fri Apr 01 2016 Mathias Lüdtke <mathias.luedtke@ipa.fraunhofer.de> - 0.6.4-0
- Autogenerated by Bloom

