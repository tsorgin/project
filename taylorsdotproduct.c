
      //////////////////////////////////////////////////////////////////////////////////////
      ///Taylor working here
      //fabs(target.Dot(axis))> max_axis_step ? max_axis_step:target.Dot(axis) is the right idea. BUt if one fails,
      //you must scale all of them by the same amount.... also have to make sure to keep sign.

      // double max_axis_step = .005;
      // vec3 axis, target;
      // target.x = camera_params.camTarget[0];
      // target.y = camera_params.camTarget[1];
      // target.z = camera_params.camTarget[2];
      // target.Normalize();

      // axis.x = 1;

      // camera_params.camPos[0] = camera_params.camPos[0] + (target.Dot(axis)*max_axis_step );

      // axis.x = 0;
      // axis.y = 1;

      // camera_params.camPos[1] = camera_params.camPos[1] + (target.Dot(axis)*max_axis_step );

      // axis.y = 0;
      // axis.z = 1;

      // camera_params.camPos[2] = camera_params.camPos[2] + (target.Dot(axis)*max_axis_step );

      // printf("%f %f %f \n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2] );
      // camera_params.camPos[0] = camera_params.camPos[0] - .025;
      // camera_params.camPos[1] = camera_params.camPos[1] - .025;
      // camera_params.camPos[2] = camera_params.camPos[2] - .025;
      ///////////////////////////////////////////////////////////////////////////////////////////