import pytest
from .conftest import GET_BOOKMARKS_URL, ADD_BOOKMARK_URL, create_bookmark_data, create_headers_json


async def test_get_bookmarks_unauthorized(service_client):
    response = await service_client.get(GET_BOOKMARKS_URL)
    assert response.status == 401


async def test_get_bookmarks_empty(service_client, register_user, login_user):
    email = "get_bookmarks_empty"
    password = "get_bookmarks_empty"
    await register_user(email, password)
    user_id = await login_user(email, password)

    headers = create_headers_json(user_id)
    response = await service_client.get(GET_BOOKMARKS_URL, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert isinstance(json_response, dict)
    assert "items" in json_response
    assert isinstance(json_response["items"], list)
    assert len(json_response["items"]) == 0


async def test_get_bookmarks_different_user(service_client, register_user, login_user, add_bookmark):
    email1 = "get_bookmarks_user1"
    password1 = "get_bookmarks_user1"
    email2 = "get_bookmarks_user2"
    password2 = "get_bookmarks_user2"

    await register_user(email1, password1)
    user_id1 = await login_user(email1, password1)

    await register_user(email2, password2)
    user_id2 = await login_user(email2, password2)

    url = "https://get_bookmarks_different_user.ru"
    title = "get_bookmarks_different_user"
    tag = "get_bookmarks_different_user"

    await add_bookmark(user_id1, url, title, tag)

    headers = create_headers_json(user_id2)
    response = await service_client.get(GET_BOOKMARKS_URL, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert isinstance(json_response, dict)
    assert "items" in json_response
    assert isinstance(json_response["items"], list)
    assert len(json_response["items"]) == 0


async def test_get_bookmarks_success(service_client, register_user, login_user, add_bookmark):
    email = "get_bookmarks_success"
    password = "get_bookmarks_success"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url1 = "https://get_bookmarks_success1.ru"
    title1 = "get_bookmarks_success1"
    tag1 = "get_bookmarks_success1"

    url2 = "https://get_bookmarks_success2.ru"
    title2 = "get_bookmarks_success2"
    tag2 = "get_bookmarks_success2"

    assert url1 != url2

    await add_bookmark(user_id, url1, title1, tag1)
    await add_bookmark(user_id, url2, title2, tag2)

    headers = create_headers_json(user_id)
    response = await service_client.get(GET_BOOKMARKS_URL, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert isinstance(json_response, dict)
    assert "items" in json_response
    assert isinstance(json_response["items"], list)

    bookmark1 = None
    bookmark2 = None

    for bookmark in json_response["items"]:
        if bookmark["url"] == url1:
            bookmark1 = bookmark
        elif bookmark["url"] == url2:
            bookmark2 = bookmark

    assert bookmark1 is not None
    assert bookmark2 is not None

    assert bookmark1["url"] == url1
    assert bookmark1["title"] == title1
    assert bookmark1["tag"] == tag1

    assert bookmark2["url"] == url2
    assert bookmark2["title"] == title2
    assert bookmark2["tag"] == tag2