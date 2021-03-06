package com.example.handlerdemo.demoGIS.handle.impl;

import com.example.handlerdemo.demoGIS.filter.SpatialFilter;
import com.example.handlerdemo.demoGIS.handle.AbstractFilterHandler;
import com.example.handlerdemo.demoGIS.model.SObject;

import java.util.List;

public class ComposeFilterHandler extends AbstractFilterHandler<List<SObject>, SpatialFilter<SObject>> {
    @Override
    public List<SObject> filter(SpatialFilter<SObject> filter) throws Exception {
        List<SObject> sObjects = data;
        System.out.println("根据是否加载组成信息，进行相应的处理");

        return sObjects;
    }
}
