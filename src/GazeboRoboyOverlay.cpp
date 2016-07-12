#include "GazeboRoboyOverlay.hpp"

using namespace gazebo;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(GazeboRoboyOverlay)

GazeboRoboyOverlay::GazeboRoboyOverlay()
        : GUIPlugin() {
    this->counter = 0;

    // Set the frame background and foreground colors
    this->setStyleSheet(
            "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

    // Create the main layout
    QHBoxLayout *mainLayout = new QHBoxLayout;

    // Create the frame to hold all the widgets
    QFrame *mainFrame = new QFrame();

    // Create the layout that sits inside the frame
    QVBoxLayout *frameLayout = new QVBoxLayout();

    QCheckBox *visualizeTendon = new QCheckBox(tr("show tendon"));
    connect(visualizeTendon, SIGNAL(clicked()), this, SLOT(showTendon()));
    frameLayout->addWidget(visualizeTendon);

    QCheckBox *visualizeForce = new QCheckBox(tr("show force"));
    connect(visualizeForce, SIGNAL(clicked()), this, SLOT(showForce()));
    frameLayout->addWidget(visualizeForce);

    QCheckBox *visualizeCOM = new QCheckBox(tr("show COM"));
    connect(visualizeCOM, SIGNAL(clicked()), this, SLOT(showCOM()));
    frameLayout->addWidget(visualizeCOM);

    // Add frameLayout to the frame
    mainFrame->setLayout(frameLayout);

    // Add the frame to the main layout
    mainLayout->addWidget(mainFrame);

    // Remove margins to reduce space
    frameLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(mainLayout);

    // Position and resize this widget
    this->move(10, 10);
    this->resize(120, 90);

    // Create a node for transportation
    this->node = transport::NodePtr(new transport::Node());
    this->node->Init();
    this->factoryPub = this->node->Advertise<msgs::Factory>("~/factory");

    if (!ros::isInitialized()) {
        int argc = 0;
        char **argv = NULL;
        ros::init(argc, argv, "GazeboRoboyOverlay",
                  ros::init_options::NoSigintHandler | ros::init_options::AnonymousName);
    }

    nh = new ros::NodeHandle;

    visualizeTendon_pub = nh->advertise<std_msgs::Bool>("/visual/visualizeTendon", 1);
    visualizeForce_pub = nh->advertise<std_msgs::Bool>("/visual/visualizeForce", 1);
    visualizeCOM_pub = nh->advertise<std_msgs::Bool>("/visual/visualizeCOM", 1);
}

GazeboRoboyOverlay::~GazeboRoboyOverlay() {
    delete nh;
}

void GazeboRoboyOverlay::OnButton() {
    msgs::Model model;
    model.set_name("plugin_unit_sphere_" + std::to_string(this->counter++));
    msgs::Set(model.mutable_pose(), ignition::math::Pose3d(0, 0, 1.5, 0, 0, 0));
    const double mass = 1.0;
    const double radius = 0.5;
    msgs::AddSphereLink(model, mass, radius);

    std::ostringstream newModelStr;
    newModelStr << "<sdf version='" << SDF_VERSION << "'>"
    << msgs::ModelToSDF(model)->ToString("")
    << "</sdf>";

    // Send the model to the gazebo server
    msgs::Factory msg;
    msg.set_sdf(newModelStr.str());
    this->factoryPub->Publish(msg);
}

void GazeboRoboyOverlay::showTendon(){
    static bool showTendonFlag = false;
    showTendonFlag = !showTendonFlag;
    std_msgs::Bool msg;
    msg.data = showTendonFlag;
    visualizeTendon_pub.publish(msg);
    ros::spinOnce();
}

void GazeboRoboyOverlay::showForce(){
    static bool showForceFlag = false;
    showForceFlag = !showForceFlag;
    std_msgs::Bool msg;
    msg.data = showForceFlag;
    visualizeForce_pub.publish(msg);
    ros::spinOnce();
}

void GazeboRoboyOverlay::showCOM(){
    static bool showCOMFlag = false;
    showCOMFlag = !showCOMFlag;
    std_msgs::Bool msg;
    msg.data = showCOMFlag;
    visualizeCOM_pub.publish(msg);
    ros::spinOnce();
}
