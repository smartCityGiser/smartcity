package com.example.redisdemo.entity;

import java.io.Serializable;
import java.util.Date;

/**
 * @author lipoGiser
 * @date 2019/11/15 14:24
 * @Version 0.1
 */
public class UserEntity implements Serializable {
    private long   id;
    private String guid;
    private String name;
    private String age;
    private Date   createTime;

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public String getGuid() {
        return guid;
    }

    public void setGuid(String guid) {
        this.guid = guid;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getAge() {
        return age;
    }

    public void setAge(String age) {
        this.age = age;
    }

    public Date getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Date createTime) {
        this.createTime = createTime;
    }
}
