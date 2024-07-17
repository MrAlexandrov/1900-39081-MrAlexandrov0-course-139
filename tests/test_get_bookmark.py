import pytest
from .conftest import GET_BOOKMARK_URL, ADD_BOOKMARK_URL, create_bookmark_data, create_headers_json


async def test_get_bookmark_success(service_client, register_user, login_user, add_bookmark):
    email = "get_success@ya.ru"
    password = "get_success"
    await register_user(email, password)
    user_id = await login_user(email, password)

    url = "https://get_success.ru"
    title = "get_success"
    tag = "get_success"
    bookmark_id = await add_bookmark(user_id, url, title, tag)

    headers = create_headers_json(user_id)
    response = await service_client.get(GET_BOOKMARK_URL.format(id=bookmark_id), headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert json_response["id"] == bookmark_id
    assert "url" in json_response
    assert json_response["url"] == url
    assert "title" in json_response
    assert json_response["title"] == title
    assert "tag" in json_response
    assert json_response["tag"] == tag
    assert "created_ts" in json_response


async def test_get_non_existing_bookmark(service_client, register_user, login_user):
    email = "non_exist@ya.ru"
    password = "non_exist"
    await register_user(email, password)
    user_id = await login_user(email, password)

    headers = create_headers_json(user_id)
    response = await service_client.get(GET_BOOKMARK_URL.format(id=9999), headers=headers)
    assert response.status == 404


async def test_get_bookmark_unauthorized(service_client):
    bookmark_id = "some_id"
    get_url = GET_BOOKMARK_URL.format(id=bookmark_id)
    response = await service_client.get(get_url)
    assert response.status == 401


async def test_get_bookmark_wrong_user(service_client, register_user, login_user):
    email1 = "firts_user@ya.ru"
    password1 = "first_password"
    email2 = "second_user@ya.ru"
    password2 = "second_password"

    await register_user(email1, password1)
    user_id1 = await login_user(email1, password1)

    url = "https://user.ru"
    title = "User"
    tag = "User"
    bookmark_data = create_bookmark_data(url, title, tag)
    headers1 = create_headers_json(user_id1)
    response = await service_client.post(ADD_BOOKMARK_URL, json=bookmark_data, headers=headers1)
    assert response.status == 200
    json_response = response.json()
    bookmark_id = json_response["id"]

    await register_user(email2, password2)
    user_id2 = await login_user(email2, password2)
    get_url = GET_BOOKMARK_URL.format(id=bookmark_id)
    headers2 = create_headers_json(user_id2)
    response = await service_client.get(get_url, headers=headers2)
    assert response.status == 404
    # TODO: add 403 status handler
